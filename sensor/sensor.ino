#define BOTAO1 18
#define BOTAO2 19

bool mostrar=false;
String mensagem;
 
/* Função ISR (chamada quando há geração da
interrupção) */
void funcao_ISR_btn1()
{
  mensagem="botao 1";
  mostrar=true;
}

void funcao_ISR_btn2()
{
  mensagem="botao 2";
  mostrar=true;
}
 
void setup()
{
Serial.begin(9600);
 
/* Configura o GPIO do botão como entrada
e configura interrupção externa no modo
RISING para ele.
*/
pinMode(BOTAO1, INPUT);
pinMode(BOTAO2, INPUT);
attachInterrupt(BOTAO1, funcao_ISR_btn1, RISING);
attachInterrupt(BOTAO2, funcao_ISR_btn2, RISING);


}
 
void loop()
{
if(mostrar){
  Serial.println(mensagem);
  mostrar=false;
}
delay(1000);
}
