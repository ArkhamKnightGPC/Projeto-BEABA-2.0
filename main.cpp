#include <bits/stdc++.h>
#include <fstream>
#include <unistd.h>
using namespace std;

#define pb push_back
#define mp make_pair
const int ALFABETO = 'z'-'a'+1;
const int MAXN = 5e3+9;
const int SAMP = 1e4+9;

int cnt=0;
vector<string> V, ultimas,trava_lingua; // V armazena texto dividido em silabas para montagem do grafo
vector<int> Escolha, verso, prov; //Escolha eh vetor usado como espaco amostral para sorteio de silabas
vector<int> comeco_anag[ALFABETO];//guardamos silabas de inicio de palavra para cada letra do alfabeto
map<string,int> M;
set<string> S; //usado para buscar palavras geradas, impedindo que sejam iguais a palavras da Train Data
map<int,string> Mrev;
double prob[MAXN][MAXN], soma[MAXN], prob_rev[MAXN][MAXN], soma_rev[MAXN]; //matriz de adjacencias e soma de linhas para calculo de probabilidades

int main(){
    setlocale(LC_ALL, "Portuguese");
	ios_base::sync_with_stdio(false);
    cin.tie(NULL);
	ifstream Arquivo; 
    string ler;
    Arquivo.open("Banco.txt");
    V.pb("/"); //barras indicam inicio e fim de palavras
	while(Arquivo >> ler){
		string silaba, palavra;
		for(int i=0; i<(int)ler.size(); i++){
			ler[i] = tolower(ler[i]); //Trabalhamos apenas com letras minusculas
			if(ler[i]=='-'){
				V.pb(silaba);
				silaba.clear();
			}else{
				if(ler[i]!='.'){
					silaba += ler[i];
					palavra += ler[i];
				}
			}
		}
		V.pb(silaba);
		ultimas.pb(silaba);//silaba final de palavra, usaremos para rimar
		V.pb("/");
		S.insert(palavra);
		ler.clear();
	}
	int barra;
	for(int i=0; i<(int)V.size(); i++){
		if(M.find(V[i])==M.end()){
			if(V[i]=="/")barra = cnt;
			M.insert(mp(V[i],cnt)); // sorteamos inteiros, depois usamos seu equivalente em string para imprimir texto
			Mrev.insert(mp(cnt,V[i]));
			cnt++;
		}
	}
	for(int i=1; i<(int)V.size(); i++){
		string s1 = V[i-1];
		string s2 = V[i];
		int v1 = M[s1]; 
		int v2 = M[s2];
		prob[v1][v2]++; soma[v1]++;
		prob_rev[v2][v1]++; soma_rev[v2]++;
		if(v1==barra && s2[0]-'a'>=0){//estamos no comeco de uma palavra, fazemos pre-processamento para geracao de anagramas
			comeco_anag[s2[0]-'a'].pb(v2);
		}
	}
	for(int i=1; i<cnt; i++){
		for(int j=1; j<cnt; j++){
			prob[i][j] = (prob[i][j]/soma[i])*SAMP; //calculamos probabilidades para sorteio
			prob_rev[i][j] = (prob_rev[i][j]/soma_rev[i])*SAMP;
		}
	}
	srand(time(NULL));
    cout<<"Bem vindo ao Projeto BEABA 2.0\n"; cout.flush();
	while(true){
    	cout<<"1 -> Gerar acronimo\n"; cout.flush();
    	cout<<"2 -> Gerar soneto\n"; cout.flush();
    	cout<<"3 -> Gerar TravaLingua\n"; cout.flush();
		cout<<"Para sair do programa, digite outro numero qualquer\n"; cout.flush();
    	cout<<"Escolha uma das funcionalidades : "; cout.flush();
    	int modo; scanf("%d", &modo);
    	if(modo>3){
			cout<<"Esperamos que tenha gostado, obrigado :)\n"; cout.flush();
			sleep(3);
			break;
		}else if(modo==1){
			string palavra;
			cout<<"Ok, Digite a palavra: "; cout.flush();
			cin>>palavra;
			cout<<"\n"; cout.flush();
			vector<string> para_salvar;
			for(int i=0; i<(int)palavra.size(); i++){
				palavra[i] = tolower(palavra[i]);
				int silaba_inicial = comeco_anag[palavra[i]-'a'][rand()%(int)comeco_anag[palavra[i]-'a'].size()];
				string silaba = Mrev[silaba_inicial];
				int atual = silaba_inicial;
				int minimo_silabas=0;
				string anagrama; anagrama.clear();
				do{
					if(atual!=barra)anagrama+=silaba;
					minimo_silabas++;
					Escolha.clear();
					for(int j=0; j<cnt; j++)
						for(int k=1; k<=prob[atual][j]; k++)
							Escolha.pb(j); //preenchemos nosso espaco amostral com base nas probabilidades
					if(Escolha.size()==0)break;
					atual = Escolha[rand()%(int)Escolha.size()];
					silaba = Mrev[atual];
				}while(atual!=barra);
				if(anagrama.size()<3 || anagrama.size()>8 || S.find(anagrama)!=S.end()){
					i--;
				}else{
					anagrama[0]=toupper(anagrama[0]);
					cout<<anagrama<<endl; cout.flush();
					para_salvar.pb(anagrama);
				}
			}
			printf("\n");
			ofstream file_;
			file_.open("TextoGerado.txt");
			for(int i=0; i<(int)para_salvar.size(); i++)
				file_<<para_salvar[i]<<"\n";
			file_.close();
			system("espeak -v pt+m1 -s 100 -p 15 -g 8 -l 10 -f Anagramas.txt");
		}else if(modo==2){
			int versos[4] = {4,4,3,3};
			
			int rima[4][4]; set<int>rimas;
			rima[0][0]= M[ultimas[rand()%(int)ultimas.size()]];
			rimas.insert(rima[0][0]);
			rima[0][3]=rima[0][0]; rima[1][0]=rima[0][0]; rima[1][3]=rima[0][0];
			
			do{rima[0][1]=M[ultimas[rand()%(int)ultimas.size()]];}while(rimas.find(rima[0][1])!=rimas.end());
			rimas.insert(rima[0][1]);
			rima[0][2]=rima[0][1]; rima[1][1]=rima[0][1]; rima[1][2]=rima[0][1];
			
			do{rima[2][0]=M[ultimas[rand()%(int)ultimas.size()]];}while(rimas.find(rima[2][0])!=rimas.end());
			rimas.insert(rima[2][0]);
			rima[2][2]=rima[2][0]; rima[3][1]=rima[2][0];
			
			do{rima[2][1]=M[ultimas[rand()%(int)ultimas.size()]];}while(rimas.find(rima[2][1])!=rimas.end());
			rima[3][0]=rima[2][1]; rima[3][2]=rima[2][1];
			
			// o esquema de rimas do soneto eh abba abba cdc dcd
			vector<string> para_salvar;
			cout<<"\n"; cout.flush();
			for(int i=0; i<4; i++){
				for(int j=0; j<versos[i]; j++){
					verso.clear();
					bool flag = true;
					int atual;
					while((int)verso.size()<10){
						if(flag)atual = rima[i][j];
						else atual = barra;
						prov.clear();
						do{
							if(atual!=barra)prov.pb(atual);
							Escolha.clear();
							for(int l=0; l<cnt; l++)
								for(int k=1; k<=prob_rev[atual][l]; k++)
									Escolha.pb(l); //preenchemos nosso espaco amostral com base nas probabilidades
							if(Escolha.size()==0)atual = barra;
							else atual = Escolha[rand()%(int)Escolha.size()];
						}while(atual!=barra);
						
						if(prov.size()>=1 && prov.size()<=5 && (int)prov.size()+(int)verso.size()<12){
							for(int k=0; k<(int)prov.size(); k++){
								verso.pb(prov[k]);
							}
							verso.pb(barra); flag=false;
						}
					}
					string aux; aux.clear();
					for(int k=(int)verso.size()-2; k>=0; k--){
						if(verso[k]==barra){aux+=" ";cout<<" ";cout.flush();continue;}
						aux += Mrev[verso[k]];
						cout<<Mrev[verso[k]];
						cout.flush();
					}
					para_salvar.pb(aux);
					cout<<endl; cout.flush();
				}
				cout<<endl; cout.flush();
			}
			ofstream file_;
			file_.open("TextoGerado.txt");
			for(int i=0; i<(int)para_salvar.size(); i++){
				if(i==4 || i==8 || i==11)file_<<"\n";
				file_<<para_salvar[i]<<"\n";
			}
			file_.close();
			system("espeak -v pt+m1 -s 100 -p 15 -g 8 -l 10 -f Sonetos.txt");
		}else if(modo==3){
			set<int> proibido; 
			proibido.insert('w'-'a'); proibido.insert('y'-'a');
			proibido.insert('x'-'a'); proibido.insert('k'-'a');
			proibido.insert('w'-'a'); proibido.insert('q'-'a');
			proibido.insert('z'-'a'); proibido.insert('j'-'a');
			proibido.insert('u'-'a');
			int letra_rep;
			do{letra_rep = rand()%ALFABETO;}while(proibido.find(letra_rep)!=proibido.end());
			int inicio_rep[20];
			for(int i=0; i<20; i++){
				inicio_rep[i] = comeco_anag[letra_rep][rand()%(int)comeco_anag[letra_rep].size()];
			}
			trava_lingua.clear();
			while((int)trava_lingua.size()<5){
				int atual = inicio_rep[rand()%20];
				string palavra; palavra.clear();
				while(true){
					if(atual==barra)break;
					palavra += Mrev[atual];
					Escolha.clear();
					for(int i=0; i<cnt; i++)
						for(int k=1; k<=prob[atual][i]; k++)
							Escolha.pb(i);
					if(Escolha.size()==0)atual = barra;
					else atual = Escolha[rand()%(int)Escolha.size()];
				}
				if(palavra.size()<6 && palavra.size()>2){
					palavra[0] = toupper(palavra[0]);
					trava_lingua.pb(palavra);
				}
			}
			for(int i=0; i<(int)trava_lingua.size(); i++){
				cout<<trava_lingua[i]<<" ";cout.flush();
			}
			cout<<endl; cout.flush();
			ofstream file_;
			file_.open("TextoGerado.txt");
			for(int i=0; i<(int)trava_lingua.size(); i++){
				file_<<trava_lingua[i]<<" ";
			}
			file_.close();
			system("espeak -v pt+m1 -s 130 -p 15 -f TravaLinguas.txt");
			cout<<endl; cout.flush();
		}
		cout<<"Obrigado por utilizar o Beaba!\n"; cout.flush();
	}
    return 0;
}
