# A1-prog2
# Trabalho de manipulação de binários pelo stdio (input/output)

# Nome: Melissa Goulart Kemp
# GRR: 20255413

Esse trabalho é de minha autoria e contém os seguintes arquivos:
	-Makefile
	-gbv.h
	-main.c
	-gbv.c 
	-util.c
	-util.h

Em que gbv.c foi implementação minha e os demais foram fornecidos pelo "A1_recursos".

Modificações feitas:

	-No header gbv.h, adicionei um novo parâmetro para as funções gbv_remove e gbv_view: o const *char archive, para remover o documento do disco na primeira função* e para visualizar todos os documentos na última. Também fiz as devidas alterações na main.c.

	-Criei uma função auxiliar no próprio gbv.c chamada gbv_find, que serve para achar em que índice está um determinado documento. Ela foi útil em três funções: no gbv_add (para procurar se já há um documento com o mesmo nome), no gbv_remove (para procurar em que índice está o documento a ser removido do gbv, se é que ele está nele) e no gbv_view (para procurar o documento que quero visualizar).
	
	-O enunciado do trabalho pede para fazer apenas uma remoção lógica, mas pelo que nos foi passado em aula, poderíamos fazer modificações no trabalho. Da forma que eu fiz, o documento a ser removido sai tanto da biblioteca quanto do arquivo físico, a fim de não estar mais lá quando eu uso a função gbv_view (cujos parâmetros também modifiquei).

 	-Coloquei no gbv.c e gbv.h as funções auxiliares da gbv_order, que são as que verificam qual é a ordem que a função qsort deve usar para reordenar os documentos binários. As funções são: int ordem_cronologica, int ordem_tamanho e int ordem_alfabetica. Elas fazem comparações de string ou tamanho pra ver qual vem primeiro e retorna o valor dependendo da função. No caso da ordenação alfabética, como é feita uma strcmp, retorna zero se forem iguais, por exemplo.
  
Dificuldades enfrentadas:

	-Os próprios conceitos do trabalho (metadados da biblioteca versus dados no arquivo físico gbv) eram muito abstratos e foi difícil de convertê-los em código. Era muito confuso saber para que servia criar o superbloco, quando escrever nele ou quando ler, e também entender a diferença entre usar uma informação que está nele ou na biblioteca (exemplo: lib->count x sb.qtd_doc).
	
	-Foram necessárias muitas verificações, porque quase todas as operações (fopen, fwrite, fread) podem dar errado. No entanto, o fato de eu ter colocado tantos testes com suas respectivas mensagens de erro me ajudou a identificar em que parte do meu código eventuais erros estavam.
	
Bugs conhecidos que NÃO consegui resolver:

	-ESTÁ FALTANDO 1 FREE (na função -a dá 10 mallocs e 9 frees, na função -v mostra 11 mallocs e 10 frees e assim por diante. Sempre aparece que falta 280 bytes para serem liberados e eu não soube como resolver.

Bugs conhecidos que eu consegui resolver:

	-(MEU ERRO MAIS FREQUENTE) Eu não estava conseguindo passar o parâmetro Library *lib corretamente porque em alguns a gente passa const Library (quando não é pra alterar a biblioteca) e outras só Library (quando tem que alterar algo), e eu não sabia disso. Na minha função gbv_find muitas vezes eu estava só copiando os parâmetros de outras funções como gbv_add e isso não dá certo, porque uma só procura um item sem alterar nada (recebe a versão constante) e a outra altera a biblioteca (recebe Library *lib).
	
	-Eu tinha esquecido de tratar o caso de o usuário querer colocar o próprio documento gbv nele mesmo, dando seg fault
		
	-Estava esquecendo de criar o arquivo gbv quando ele ainda não existia
	
	-Não estava falando que lib->docs = NULL quando lib->count era zero, então eu tentava fazer malloc para ele e multiplicava o sizeof pela qtd de documentos de lib (lib->count), e isso dava zero. Assim, não conseguia alocar memória (foi fácil de descobrir esse erro justamente porque eu coloquei muitas mensagens de erro).
