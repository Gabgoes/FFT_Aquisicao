#include <iostream>
#include <fstream>
#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>
#include <fftw3.h>
#include <cmath>
#include <iomanip>

// define REAL e IMAGINÁRIO
#define REAL 0
#define IMAG 1

using namespace std;

int main()
{

	// troca idioma de exibição
	_tsetlocale(LC_ALL, _T("portuguese"));

	// cria a classe 'arquivo' como fstream para manipular o txt tanto leitura como escrita
	fstream arquivo;
	// cria a classe 'ss' para manipular uma linha de string
	stringstream ss, ss_aux;

	// cria a classe string 'linha' para manipular as linhas do txt
	string linha;
	// cria as classes string para poder separar os dados de cada eixo
	string x_string, y_string, z_string;

	// cria os vetores para salvar os dados de aceleração de cada eixo
	vector <double> x;
	vector <double> y;
	vector <double> z;

	// variável auxiliar utilizada para armazenar a conversão de string para double
	double aux_converter;


	/******************************* Leitura do Arquivo .txt *******************************/

	// abre o arquivo txt com os dados aquisitados pelo acelerômetro
	arquivo.open("1602245833-2715-NAO7856.txt",ios::in);

	if (arquivo.is_open())
	{
		// faz a leitura de cada linha
		while (getline(arquivo, linha))
		{

			//salva a linha na classe 'ss'
			stringstream ss(linha);
			// separa cada linha em eixo x, y e z
			getline(ss, x_string, ',');
			getline(ss, y_string, ',');
			getline(ss, z_string, ',');
			
			// converte de tipo string para tipo double
			ss_aux << x_string;
			ss_aux >> aux_converter;
			ss_aux.clear();
			// adiciona no vetor do eixo x
			x.push_back(aux_converter);
			
			// converte de tipo string para tipo double
			ss_aux << y_string;
			ss_aux >> aux_converter;
			ss_aux.clear();
			// adiciona no vetor do eixo x
			y.push_back(aux_converter);

			// converte de tipo string para tipo double
			ss_aux << z_string;
			ss_aux >> aux_converter;
			ss_aux.clear();
			// adiciona no vetor do eixo x
			z.push_back(aux_converter);

		}
		// fecha o arquivo txt
		arquivo.close();
	}
		// caso não consiga abrir o arquivo txt exibe a seguinte mensagem de erro
		else
		{
			std::cout << "Não foi possível abrir o arquivo" << std::endl;
		}


	/******************************* Cálculo das FFTs *******************************/


	// define o tamanho dos sinais de entrada, saída e número de pontos da FFT
	const int N = 4139;
	
	// declara as classes de entrada e saída para cada eixo 
	fftw_complex x_FFT_in[N];
	fftw_complex x_FFT_out[N];

	fftw_complex y_FFT_in[N];
	fftw_complex y_FFT_out[N];

	fftw_complex z_FFT_in[N];
	fftw_complex z_FFT_out[N];

	// atribui valores ao vetor de entrada, nas partes real e imaginária
	for (int i = 0; i < N; i++)
	{
		x_FFT_in[i][REAL] = x[i];
		x_FFT_in[i][IMAG] = 0;

		y_FFT_in[i][REAL] = y[i];
		y_FFT_in[i][IMAG] = 0;

		z_FFT_in[i][REAL] = z[i];
		z_FFT_in[i][IMAG] = 0;
	}

	// declara os planos para realizar o cálculo das FFTs
	fftw_plan plan_x, plan_y, plan_z;

	// cria os planos das FFTs de 1 dimensão
	plan_x = fftw_plan_dft_1d(N, x_FFT_in, x_FFT_out, FFTW_FORWARD, FFTW_ESTIMATE);
	plan_y = fftw_plan_dft_1d(N, y_FFT_in, y_FFT_out, FFTW_FORWARD, FFTW_ESTIMATE);
	plan_z = fftw_plan_dft_1d(N, z_FFT_in, z_FFT_out, FFTW_FORWARD, FFTW_ESTIMATE);

	// executa o cálculo das FFTs
	fftw_execute(plan_x);
	fftw_execute(plan_y);
	fftw_execute(plan_z);

	// apaga e limpa os planos criados
	fftw_destroy_plan(plan_x);
	fftw_destroy_plan(plan_y);
	fftw_destroy_plan(plan_z);
	fftw_cleanup();


	/******************************* Escrita no Arquivo .txt de Saída *******************************/


	// frequência de amostragem fs = 4139 amostras / 2.715 segundos = 1524.49 amostras/segundo = 1524.49 Hz
	double fs = N / 2.715;

	// cria o vetor de frequencias
	vector <double> freq;

	// salvando os resultados (módulos da FFT de cada eixo) no arquivo output.txt
	arquivo.open("output.txt",ios::out);

	// variáveis para salvar os módulos para cada ponto de frequência
	double modulo_x, modulo_y, modulo_z;

	// for para realizar os cálculos dos módulos de cada ponto de frequência
	for (int i = 0; i < N; i++) {
		
		// modulo(Z) = sqrt( (real(Z))^2 + (imag(Z))^2 )
		modulo_x = sqrt(pow(x_FFT_out[i][REAL], 2) + pow(x_FFT_out[i][IMAG], 2));
		modulo_y = sqrt(pow(y_FFT_out[i][REAL], 2) + pow(y_FFT_out[i][IMAG], 2));
		modulo_z = sqrt(pow(z_FFT_out[i][REAL], 2) + pow(z_FFT_out[i][IMAG], 2));

		// calcula a frequência e adiciona no vetor de frequências
		freq.push_back((fs/N)*i);

		// escreve no arquivo de saída
		arquivo << setprecision(12) << modulo_x << "," << setprecision(12) << modulo_y << "," << setprecision(12) << modulo_z << "," << freq[i] << "\n";

	}
 
	// fecha o arquivo txt
	arquivo.close();
	
	system("pause");
	return 0;
}