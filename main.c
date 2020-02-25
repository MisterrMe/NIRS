void summ(short *out){
   short i, buf = 0;
   for (i=7; i>0; i--){
      out[i] = (out[i] + out[i+8] + buf) % 256;
      buf = (out[i] + out[i+8]) / 256;
   }
}

void xor(short *inp1, short *inp2){
   short i;
   for (i=0; i<8; i++){
      inp1[i].B0 = inp1[i].B0 ^ inp2[i].B0;
      inp1[i].B1 = inp1[i].B1 ^ inp2[i].B1;
      inp1[i].B2 = inp1[i].B2 ^ inp2[i].B2;
      inp1[i].B3 = inp1[i].B3 ^ inp2[i].B3;
      inp1[i].B4 = inp1[i].B4 ^ inp2[i].B4;
      inp1[i].B5 = inp1[i].B5 ^ inp2[i].B5;
      inp1[i].B6 = inp1[i].B6 ^ inp2[i].B6;
      inp1[i].B7 = inp1[i].B7 ^ inp2[i].B7;
   }
}

void round_encr(short *out, short *key){
   short i, buf;
   
   //Сдвиг >>>8
   buf = out[7];
   for (i=7; i>0; i--){
      out[i]=out[i-1];
   }
   out[0] = buf;

   //Сложение по mod 2^n
   summ(out);
   
      //Xor с ключом
   xor(out,key);

   //Сдвиг <<< 3
   buf = out[8];
   for (i=8; i<15; i++){
      out[i].B0 = out[i].B3;
      out[i].B1 = out[i].B4;
      out[i].B2 = out[i].B5;
      out[i].B3 = out[i].B6;
      out[i].B4 = out[i].B7;
      out[i].B5 = out[i+1].B0;
      out[i].B6 = out[i+1].B1;
      out[i].B7 = out[i+1].B2;
   }
   out[8].B0 = out[8].B3;
   out[8].B1 = out[8].B4;
   out[8].B2 = out[8].B5;
   out[8].B3 = out[8].B6;
   out[8].B4 = out[8].B7;
   out[8].B5 = buf.B0;
   out[8].B6 = buf.B1;
   out[8].B7 = buf.B2;
   
   //Xor
   xor(out+8,out);
}

void round_decr(short *out, short *key){
   short i, buf;
   
   //Xor
   xor(out+8,out);
   
   //Xor с ключом
   xor(out,key);
   
   //Сдвиг >>> 3
   buf = out[15];
   for (i=15; i>8; i--){
      out[i].B0 = out[i-1].B5;
      out[i].B1 = out[i-1].B6;
      out[i].B2 = out[i-1].B7;
      out[i].B3 = out[i].B0;
      out[i].B4 = out[i].B1;
      out[i].B5 = out[i].B2;
      out[i].B6 = out[i].B3;
      out[i].B7 = out[i].B4;
   }
   out[8].B0 = buf.B5;
   out[8].B1 = buf.B6;
   out[8].B2 = buf.B7;
   out[8].B3 = out[8].B0;
   out[8].B4 = out[8].B1;
   out[8].B5 = out[8].B2;
   out[8].B6 = out[8].B3;
   out[8].B7 = out[8].B4;
   
   //Обратн
   out[15]=256-out[15];
   for (i=8;i<15;i++){
      out[i]=255-out[i];
   }
   
   //Сложение по mod 2^n
   summ(out);
   
   //Сдвиг <<<8
   buf = out[0];
   for (i=0; i<7; i++){
      out[i]=out[i+1];
   }
   out[7] = buf;
}

void main() {
   short out[16] = {0b11001010,0b10011010,0b11010100,0b10110011,
                    0b10010111,0b00110010,0b01001110,0b10100110,
                    0b01010101,0b10001010,0b00101110,0b11110100,
                    0b10000011,0b11100100,0b10000100,0b01000111};
   short key[8] = {0b01010111,0b11010111,0b01001100,0b11101010,
                    0b11101010,0b11101010,0b11011110,0b10101101};
   short i,j;

   TRISB = 0b01111111; //comment
   UART1_Init(9600);

   while(1){
      if (RB7_BIT == 1 && RB0_BIT == 0 && RB1_BIT == 0){
         RB7_BIT = 0;
         delay_ms(1000);
         continue;
      }
      if (RB7_BIT == 0 && RB0_BIT == 0 && RB1_BIT == 0){
         RB7_BIT = 1;
         delay_ms(1000);
         continue;
      }

      if (RB0_BIT == 1){

         for (i=0;i<8;i++){
            UART1_Write(out[i]);
         }

         for (i=0;i<8;i++){
            UART1_Write(0x00);
         }
         
         round_encr(out,key);

         for (i=0;i<8;i++){
            UART1_Write(out[i]);
         }

         for (i=0;i<8;i++){
            UART1_Write(0x00);
         }
         
         round_decr(out,key);
         
         for (i=0;i<8;i++){
            UART1_Write(out[i]);
         }
         
         for (i=0;i<8;i++){
            UART1_Write(0x00);
         }

         while(1){
         if (RB0_BIT == 0)
            break;
         }
      }
   }
}
