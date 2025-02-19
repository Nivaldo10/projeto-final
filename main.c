#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FUNCIONARIOS 100
#define FORMATO_DATA "%Y-%m-%d"

typedef struct {
    int id;
    char nome[50];
    int idade;
    int dependentes;
    float salario;
    char cargo[50];
    char data_admissao[11]; // Formato YYYY-MM-DD
} Funcionario;

// Função para calcular o tempo de serviço em anos
int calcular_tempo_servico(const char* data_admissao) {
    time_t agora = time(NULL);
    struct tm admissao_tm;
    strptime(data_admissao, FORMATO_DATA, &admissao_tm);
    time_t tempo_admissao = mktime(&admissao_tm);
    double diferenca = difftime(agora, tempo_admissao) / (60 * 60 * 24 * 365.25);
    return (int)diferenca;
}

// Função para comparar funcionários por salário
int comparar_salario(const void* a, const void* b) {
    Funcionario* func1 = (Funcionario*)a;
    Funcionario* func2 = (Funcionario*)b;
    return (func1->salario > func2->salario) - (func1->salario < func2->salario);
}

// Função para comparar funcionários por idade
int comparar_idade(const void* a, const void* b) {
    Funcionario* func1 = (Funcionario*)a;
    Funcionario* func2 = (Funcionario*)b;
    return func1->idade - func2->idade;
}

// Função para comparar funcionários por número de dependentes
int comparar_dependentes(const void* a, const void* b) {
    Funcionario* func1 = (Funcionario*)a;
    Funcionario* func2 = (Funcionario*)b;
    return func1->dependentes - func2->dependentes;
}

// Função para comparar funcionários por tempo de serviço
int comparar_tempo_servico(const void* a, const void* b) {
    Funcionario* func1 = (Funcionario*)a;
    Funcionario* func2 = (Funcionario*)b;
    int anos1 = calcular_tempo_servico(func1->data_admissao);
    int anos2 = calcular_tempo_servico(func2->data_admissao);
    return anos1 - anos2;
}

int main() {
    FILE* arquivo = fopen("func.txt", "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    Funcionario funcionarios[MAX_FUNCIONARIOS];
    int n = 0;

    // Ler o arquivo
    char linha[256];
    fgets(linha, sizeof(linha), arquivo); // Ignorar o cabeçalho
    while (fgets(linha, sizeof(linha), arquivo)) {
        // Verifica se os dados são válidos antes de fazer o sscanf
        int id, idade, dependentes;
        float salario;
        char nome[50], cargo[50], data_admissao[11];

        int campos_lidos = sscanf(linha, "%d;%49[^;];%d;%d;R$%f;%49[^;];%10s",
            &id, nome, &idade, &dependentes, &salario, cargo, data_admissao);

        // Só processa a linha se todos os campos forem lidos corretamente
        if (campos_lidos == 7) {
            funcionarios[n].id = id;
            strcpy(funcionarios[n].nome, nome);
            funcionarios[n].idade = idade;
            funcionarios[n].dependentes = dependentes;
            funcionarios[n].salario = salario;
            strcpy(funcionarios[n].cargo, cargo);
            strcpy(funcionarios[n].data_admissao, data_admissao);
            n++;
        }
    }
    fclose(arquivo);

    // Tarefa 1: Conceder um bônus de 5% para funcionários elegíveis
    for (int i = 0; i < n; i++) {
        if (funcionarios[i].salario <= 2500.00 || funcionarios[i].dependentes >= 2) {
            funcionarios[i].salario *= 1.05;
        }
    }

    // Tarefa 2: Contar funcionários com pelo menos 3 anos de serviço
    int contador_tempo_servico = 0;
    for (int i = 0; i < n; i++) {
        if (calcular_tempo_servico(funcionarios[i].data_admissao) >= 3) {
            contador_tempo_servico++;
        }
    }

    // Tarefa 3: Calcular a faixa salarial
    float salario_minimo = funcionarios[0].salario;
    float salario_maximo = funcionarios[0].salario;
    for (int i = 1; i < n; i++) {
        if (funcionarios[i].salario < salario_minimo) salario_minimo = funcionarios[i].salario;
        if (funcionarios[i].salario > salario_maximo) salario_maximo = funcionarios[i].salario;
    }
    float faixa_salarial = salario_maximo - salario_minimo;

    // Tarefa 4: Calcular a média aritmética simples dos salários
    float total_salarios = 0;
    for (int i = 0; i < n; i++) {
        total_salarios += funcionarios[i].salario;
    }
    float media_salarial = total_salarios / n;

    // Tarefa 5: Distribuição de frequência dos salários
    int frequencia[5] = {0}; // Intervalos de frequência
    float intervalos[5][2] = {
        {0, 1518}, {1518, 3036}, {3036, 7590}, {7590, 9108}, {9108, 10626}
    };
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 5; j++) {
            if (funcionarios[i].salario >= intervalos[j][0] && funcionarios[i].salario < intervalos[j][1]) {
                frequencia[j]++;
                break;
            }
        }
    }

    // Tarefa 6: Salário modal
    int frequencia_maxima = 0;
    float salario_modal = 0;
    for (int i = 0; i < 5; i++) {
        if (frequencia[i] > frequencia_maxima) {
            frequencia_maxima = frequencia[i];
            salario_modal = (intervalos[i][0] + intervalos[i][1]) / 2;
        }
    }

    // Exibir resultados
    printf("Número de funcionários com pelo menos 3 anos de serviço: %d\n", contador_tempo_servico);
    printf("Faixa salarial: R$%.2f\n", faixa_salarial);
    printf("Média salarial: R$%.2f\n", media_salarial);
    printf("Salário modal: R$%.2f\n", salario_modal);
    printf("Distribuição de frequência dos salários:\n");
    for (int i = 0; i < 5; i++) {
        printf("%.2f - %.2f: %d funcionários\n", intervalos[i][0], intervalos[i][1], frequencia[i]);
    }

    // Menu para ordenar funcionários
    int escolha;
    do {
        printf("\nOrdenar funcionários por:\n");
        printf("1. Salário\n");
        printf("2. Idade\n");
        printf("3. Dependentes\n");
        printf("4. Tempo de serviço\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                qsort(funcionarios, n, sizeof(Funcionario), comparar_salario);
                break;
            case 2:
                qsort(funcionarios, n, sizeof(Funcionario), comparar_idade);
                break;
            case 3:
                qsort(funcionarios, n, sizeof(Funcionario), comparar_dependentes);
                break;
            case 4:
                qsort(funcionarios, n, sizeof(Funcionario), comparar_tempo_servico);
                break;
            case 5:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                continue;
        }

        if (escolha >= 1 && escolha <= 4) {
            printf("\nFuncionários Ordenados:\n");
            printf("ID\tNome\t\tIdade\tDependentes\tSalário\tCargo\tData de Admissão\n");
            for (int i = 0; i < n; i++) {
                printf("%d\t%s\t%d\t%d\t\tR$%.2f\t%s\t%s\n",
                       funcionarios[i].id, funcionarios[i].nome, funcionarios[i].idade,
                       funcionarios[i].dependentes, funcionarios[i].salario,
                       funcionarios[i].cargo, funcionarios[i].data_admissao);
            }
        }
    } while (escolha != 5);

    return 0;
}
