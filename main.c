#include <stdio.h>
#include <stdlib.h>
#include "/usr/include/postgresql/libpq-fe.h"
#include <string.h>

static void exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

void deletarTabelas(PGconn *conn)
{
    char tableName[100];
    printf("Digite o nome da tabela a ser deletada: ");
    scanf("%s", tableName);
    char query[200];
    sprintf(query, "DROP TABLE IF EXISTS %s;", tableName);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        exit_nicely(conn);
    }
    PQclear(res);
}

void listarTabelas(PGconn *conn)
{
    PGresult *res = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public';");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Failed to get tables: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int num_tables = PQntuples(res);
    printf("Numero de tabelas: %d\n", num_tables);
    for (int i = 0; i < num_tables; i++)
    {
        printf("%s\n", PQgetvalue(res, i, 0));
    }
    PQclear(res);
}

void createTable(PGconn *conn)
{
    char tableName[100];
    PGresult *res;
    char queryTable[200];
    printf("Digite o nome da nova tabela: ");
    scanf("%s", tableName);

    sprintf(queryTable, "CREATE TABLE %s();", tableName);
    printf("%s\n", queryTable);
    res = PQexec(conn, queryTable);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        exit_nicely(conn);
    }
    PQclear(res);
}

void inserirColunas(PGconn *conn)
{
    PGresult *res;
    char option;
    char tableName[100];
    printf("Digite o nome da tabela que deseja inserir: ");
    scanf("%s", tableName);

    do
    {
        char columnName[100];
        char columnType[100];
        printf("Digite o nome da coluna: \n");
        scanf("%s", columnName);

        printf("Digite o tipo da coluna: \n");
        scanf("%s", columnType);

        char query[500];
        sprintf(query, "ALTER TABLE %s ADD COLUMN %s %s;", tableName, columnName, columnType);
        res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            exit_nicely(conn);
        }
        PQclear(res);

        printf("Deseja adicionar mais uma coluna? (s/n)\n");
        scanf("%c", &option);
    } while (option == 's');
}

void inserirDados(PGconn *conn)
{
    char tableName[100];
    printf("Digite o nome da tabela: \n");
    scanf("%s", tableName);
    char query[200];
    sprintf(query, "SELECT * FROM %s;", tableName);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Failed to get tables: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int num_columns = PQnfields(res);
    printf("Colunas: ");
    for (int i = 0; i < num_columns; i++)
    {
        printf("%s ", PQfname(res, i));
    }
    printf("\n");
    PQclear(res);
    printf("Digite os dados a serem inseridos: \nExemplo: '1, 'João', 'Rua 1', '123456789'\n");
    char dados[200];
    scanf("%s", dados);
    char queryInsert[500];
    sprintf(queryInsert, "INSERT INTO %s VALUES (%s);", tableName, dados);
    res = PQexec(conn, queryInsert);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        exit_nicely(conn);
    }
    PQclear(res);
}

void especificacoes(PGconn *conn)
{
    int i,j;
    char tableName[100];
    printf("Digite o nome da tabela: ");
    scanf("%s", tableName);
    char query[500];
    sprintf(query, "SELECT column_name, data_type FROM information_schema.columns WHERE table_schema = 'public' AND table_name = '%s';", tableName);
    //printf("%s\n", query);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Failed to get tables: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int nFields = PQnfields(res);
    for (i = 0; i < nFields; i++)
        printf("%-15s", PQfname(res, i));
    printf("\n\n");

    /* next, print out the rows */
    for (i = 0; i < PQntuples(res); i++)
    {
        for (j = 0; j < nFields; j++)
            printf("%-15s", PQgetvalue(res, i, j));
        printf("\n");
    }
    // int num_columns = PQnfields(res);
    // for (int i = 0; i < num_columns; i++)
    // {
    //     printf("%s %s \n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
    // }

    PQclear(res);
}

int main(int argc, char const *argv[])
{
    PGresult *res;
    char conninfo[300];
    int option = 0;

    for (int aux = 1; aux < argc; aux++)
    {
        // printf("%s\n", argv[aux]);
        strcat(conninfo, argv[aux]);
        strcat(conninfo, " ");
    }
    // printf("%s", conninfo);

    PGconn *conn;
    conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit_nicely(conn);
    }
    printf("Certo\n\n\n");

    printf("--Sistema de Banco de Dados Postgres--\n\n");
    printf("1 - Exibir lista de tabelas\n");
    printf("2 - Especificações de campos\n");
    printf("3 - Criar nova tabela\n");
    printf("4 - Inserir dados\n");
    printf("5 - Exibir dados\n");
    printf("6 - Remover dados\n");
    printf("7 - Remover tabelas\n");
    printf("8 - Inserir colunas\n");
    printf("9 - Sair\n");
    do
    {
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            listarTabelas(conn);
            break;
        case 2:
            especificacoes(conn);
            break;
        case 3:
            createTable(conn);
            break;
        case 4:
            inserirDados(conn);
            break;
        case 5:

            break;
        case 6:
            /* code */
            break;
        case 7:
            deletarTabelas(conn);
            break;
        case 8:
            inserirColunas(conn);
            break;
        case 9:
            return 0;
            break;
        default:
            break;
        }
    } while (option != 7);
}
