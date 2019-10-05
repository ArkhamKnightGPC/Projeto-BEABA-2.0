#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <vector>
#include <set>
#include <thread>

template <typename Tipo, int Colunas, int Linhas>
using matriz = std::array<std::array<Tipo, Colunas>, Linhas>;

template <typename Tipo, int Comprimento>
using vetor = std::array<Tipo, Comprimento>;

template <typename Tipo>
constexpr auto maxNum = []() { return std::numeric_limits<Tipo>::max(); };

enum OpcaoMenu : int {
    Acronimo = 1,
    Soneto = 2,
    TravaLingua = 3
};

OpcaoMenu MenuPrincipal() {
    std::cout << "1 -> Gerar acronimo\n";
    std::cout.flush();
    std::cout << "2 -> Gerar soneto\n";
    std::cout.flush();
    std::cout << "3 -> Gerar TravaLingua\n";
    std::cout.flush();
    std::cout << "Para sair do programa, digite outro numero qualquer\n";
    std::cout.flush();
    std::cout << "Escolha uma das funcionalidades : ";
    std::cout.flush();
    OpcaoMenu modo;
    scanf("%d", &modo);
    return modo;
}

int main()
{
	constexpr size_t ALFABETO = 'z' - 'a' + 1;
	constexpr size_t MAXN = 4e3 + 9;
	constexpr size_t SAMP = 1e4 + 9;

	int cnt = 0;
	std::vector<std::string> silabas, ultimas, trava_lingua;   // silabas armazena texto dividido em silabas para montagem do grafo
    std::vector<int> Escolha, verso, prov;                 //Escolha eh vetor usado como espaco amostral para sorteio de silabas
    std::array<std::vector<int>, ALFABETO> comeco_anag;	 //guardamos silabas de inicio de palavra para cada letra do alfabeto
	std::map<std::string, int> M;
	std::set<std::string> universoDePalavras;
	std::map<int, std::string> Mrev;
    static matriz<double, MAXN, MAXN> prob, prob_rev; //matriz de adjacencias
    static vetor<double, MAXN> soma, soma_rev;        // e soma de linhas para calculo de probabilidades

	//setlocale(LC_ALL, "Portuguese");
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::ifstream Arquivo;
	std::string ler;
	
	Arquivo.open("Banco.txt");
    if (Arquivo.fail()) {
        std::cout << "Erro: Banco.txt não econtrado!" << std::endl;
        return 1;
    }

	silabas.push_back("/"); //barras indicam inicio e fim de palavras

	while (Arquivo >> ler)
	{
		std::string silaba, palavra;
		for (char letra : ler)
		{
			letra = tolower(letra); //Trabalhamos apenas com letras minusculas
			if (letra == '-')
			{
				silabas.push_back(silaba);
				silaba.clear();
			}
			else
			{
				if (letra != '.')
				{
					silaba += letra;
					palavra += letra;
				}
			}
		}
		silabas.push_back(silaba);
		ultimas.push_back(silaba); //silaba final de palavra, usaremos para rimar
		silabas.push_back("/");
		universoDePalavras.insert(palavra);
		ler.clear();
	}

    int barra = maxNum<int>();

	for (auto palavra : silabas)
	{
		if (M.find(palavra) == M.end())
		{
			if (palavra == "/")
				barra = cnt;
			M.insert(std::make_pair(palavra, cnt)); // sorteamos inteiros, depois usamos seu equivalente em std::string para imprimir texto
			Mrev.insert(std::make_pair(cnt, palavra));
			cnt++;
		}
	}

	for (int i = 1; i < (int)silabas.size(); i++)
	{
		std::string s1 = silabas[i - 1];
		std::string s2 = silabas[i];
		int v1 = M[s1];
		int v2 = M[s2];
		prob[v1][v2]++;
		soma[v1]++;
		prob_rev[v2][v1]++;
		soma_rev[v2]++;
		if (v1 == barra && s2[0] - 'a' >= 0)
		{ //estamos no comeco de uma palavra, fazemos pre-processamento para geracao de anagramas
			comeco_anag[s2[0] - 'a'].push_back(v2);
		}
	}

	for (int i = 1; i < cnt; i++)
	{
		for (int j = 1; j < cnt; j++)
		{
			prob[i][j] = (prob[i][j] / soma[i]) * SAMP; //calculamos probabilidades para sorteio
			prob_rev[i][j] = (prob_rev[i][j] / soma_rev[i]) * SAMP;
		}
	}

	srand(time(NULL));
	std::cout << "Bem vindo ao Projeto BEABA 2.0\n";
	std::cout.flush();

	for (;;)
	{
        switch(MenuPrincipal())
        {
            case Acronimo: {
                std::string palavra;
                std::cout << "Ok, Digite a palavra: ";
                std::cout.flush();
                std::cin >> palavra;
                std::cout << "\n";
                std::cout.flush();
                std::vector<std::string> para_salvar;
                for (int i = 0; i < (int)palavra.size(); i++)
                {
                    palavra[i] = tolower(palavra[i]);
                    int silaba_inicial = comeco_anag[palavra[i] - 'a'][rand() % (int)comeco_anag[palavra[i] - 'a'].size()];
                    std::string silaba = Mrev[silaba_inicial];
                    int atual = silaba_inicial;
                    int minimo_silabas = 0;
                    std::string anagrama;
                    anagrama.clear();
                    do
                    {
                        if (atual != barra)
                            anagrama += silaba;
                        minimo_silabas++;
                        Escolha.clear();
                        for (int j = 0; j < cnt; j++)
                            for (int k = 1; k <= prob[atual][j]; k++)
                                Escolha.push_back(j); //preenchemos nosso espaco amostral com base nas probabilidades
                        if (Escolha.size() == 0)
                            break;
                        atual = Escolha[rand() % (int)Escolha.size()];
                        silaba = Mrev[atual];
                    } while (atual != barra);
                    if (anagrama.size() < 3 || anagrama.size() > 8 || universoDePalavras.find(anagrama) != universoDePalavras.end())
                    {
                        i--;
                    }
                    else
                    {
                        anagrama[0] = toupper(anagrama[0]);
                        std::cout << anagrama << std::endl;
                        std::cout.flush();
                        para_salvar.push_back(anagrama);
                    }
                }
                printf("\n");
                std::ofstream file_;
                file_.open("TextoGerado.txt");
                for (int i = 0; i < (int)para_salvar.size(); i++)
                    file_ << para_salvar[i] << "\n";
                file_.close();
                system("espeak -v pt+m1 -s 100 -p 15 -g 8 -l 10 -f TextoGerado.txt");
            }
            break;
                
            case Soneto: {
                int versos[4] = {4, 4, 3, 3};

                int rima[4][4];
                std::set<int> rimas;
                rima[0][0] = M[ultimas[rand() % (int)ultimas.size()]];
                rimas.insert(rima[0][0]);
                rima[0][3] = rima[0][0];
                rima[1][0] = rima[0][0];
                rima[1][3] = rima[0][0];

                do
                {
                    rima[0][1] = M[ultimas[rand() % (int)ultimas.size()]];
                } while (rimas.find(rima[0][1]) != rimas.end());
                rimas.insert(rima[0][1]);
                rima[0][2] = rima[0][1];
                rima[1][1] = rima[0][1];
                rima[1][2] = rima[0][1];

                do
                {
                    rima[2][0] = M[ultimas[rand() % (int)ultimas.size()]];
                } while (rimas.find(rima[2][0]) != rimas.end());
                rimas.insert(rima[2][0]);
                rima[2][2] = rima[2][0];
                rima[3][1] = rima[2][0];

                do
                {
                    rima[2][1] = M[ultimas[rand() % (int)ultimas.size()]];
                } while (rimas.find(rima[2][1]) != rimas.end());
                rima[3][0] = rima[2][1];
                rima[3][2] = rima[2][1];

                // o esquema de rimas do soneto eh abba abba cdc dcd
                std::vector<std::string> para_salvar;
                std::cout << "\n";
                std::cout.flush();
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < versos[i]; j++)
                    {
                        verso.clear();
                        bool flag = true;
                        int atual;
                        while ((int)verso.size() < 10)
                        {
                            if (flag)
                                atual = rima[i][j];
                            else
                                atual = barra;
                            prov.clear();
                            do
                            {
                                if (atual != barra)
                                    prov.push_back(atual);
                                Escolha.clear();
                                for (int l = 0; l < cnt; l++)
                                    for (int k = 1; k <= prob_rev[atual][l]; k++)
                                        Escolha.push_back(l); //preenchemos nosso espaco amostral com base nas probabilidades
                                if (Escolha.size() == 0)
                                    atual = barra;
                                else
                                    atual = Escolha[rand() % (int)Escolha.size()];
                            } while (atual != barra);

                            if (prov.size() >= 1 && prov.size() <= 5 && (int)prov.size() + (int)verso.size() < 12)
                            {
                                for (int k = 0; k < (int)prov.size(); k++)
                                {
                                    verso.push_back(prov[k]);
                                }
                                verso.push_back(barra);
                                flag = false;
                            }
                        }
                        std::string aux;
                        aux.clear();
                        for (int k = (int)verso.size() - 2; k >= 0; k--)
                        {
                            if (verso[k] == barra)
                            {
                                aux += " ";
                                std::cout << " ";
                                std::cout.flush();
                                continue;
                            }
                            aux += Mrev[verso[k]];
                            std::cout << Mrev[verso[k]];
                            std::cout.flush();
                        }
                        para_salvar.push_back(aux);
                        std::cout << std::endl;
                        std::cout.flush();
                    }
                    std::cout << std::endl;
                    std::cout.flush();
                }
                std::ofstream file_;
                file_.open("TextoGerado.txt");
                for (int i = 0; i < (int)para_salvar.size(); i++)
                {
                    if (i == 4 || i == 8 || i == 11)
                        file_ << "\n";
                    file_ << para_salvar[i] << "\n";
                }
                file_.close();
                system("espeak -v pt+m1 -s 100 -p 15 -g 8 -l 10 -f TextoGerado.txt");
            }
            break;

            case TravaLingua: {
                std::set<int> proibido;
                proibido.insert('w' - 'a');
                proibido.insert('y' - 'a');
                proibido.insert('x' - 'a');
                proibido.insert('k' - 'a');
                proibido.insert('w' - 'a');
                proibido.insert('q' - 'a');
                proibido.insert('z' - 'a');
                proibido.insert('j' - 'a');
                proibido.insert('u' - 'a');
                proibido.insert('o' - 'a');
                proibido.insert('e' - 'a');
                proibido.insert('h' - 'a');
                proibido.insert('i' - 'a');
                proibido.insert('a' - 'a');
                int letra_rep;
                do
                {
                    letra_rep = rand() % ALFABETO;
                } while (proibido.find(letra_rep) != proibido.end());
                int inicio_rep[20];
                for (int i = 0; i < 20; i++)
                {
                    inicio_rep[i] = comeco_anag[letra_rep][rand() % (int)comeco_anag[letra_rep].size()];
                }
                trava_lingua.clear();
                std::set<std::string> ja_foi;
                while ((int)trava_lingua.size() < 5)
                {
                    int atual = inicio_rep[rand() % 20];
                    std::string palavra;
                    palavra.clear();
                    while (true)
                    {
                        if (atual == barra)
                            break;
                        palavra += Mrev[atual];
                        Escolha.clear();
                        for (int i = 0; i < cnt; i++)
                            for (int k = 1; k <= prob[atual][i]; k++)
                                Escolha.push_back(i);
                        if (Escolha.size() == 0)
                            atual = barra;
                        else
                            atual = Escolha[rand() % (int)Escolha.size()];
                    }
                    if (palavra.size() < 6 && palavra.size() > 2 && ja_foi.find(palavra) == ja_foi.end())
                    {
                        ja_foi.insert(palavra);
                        palavra[0] = toupper(palavra[0]);
                        trava_lingua.push_back(palavra);
                    }
                }
                for (int i = 0; i < (int)trava_lingua.size(); i++)
                {
                    std::cout << trava_lingua[i] << " ";
                    std::cout.flush();
                }
                std::cout << std::endl;
                std::cout.flush();
                std::ofstream file_;
                file_.open("TextoGerado.txt");
                for (int i = 0; i < (int)trava_lingua.size(); i++)
                {
                    file_ << trava_lingua[i] << " ";
                }
                file_.close();
                system("espeak -v pt+m1 -s 130 -p 15 -f TextoGerado.txt");
                std::cout << std::endl;
                std::cout.flush();
            }
            break;
            
            default: {
                std::cout << "Esperamos que tenha gostado, obrigado :)\n";
                std::cout.flush();

				using namespace std::chrono_literals;
				std::this_thread::sleep_for(3s);
				
                return 0;
            }
        }
        
		std::cout << "Obrigado por utilizar o Beaba!\n";
		std::cout.flush();
	}
	return 0;
}
