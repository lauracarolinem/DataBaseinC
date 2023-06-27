#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"
#include <string.h>

static void exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
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
    printf("Number of tables: %d\n", num_tables);
    for (int i = 0; i < num_tables; i++)
    {
        printf("%s\n", PQgetvalue(res, i, 0));
    }
    PQclear(res);
}

void createTable(PGconn *conn)
{
    char tableName[100];
    printf("Digite o nome da nova tabela: \n");
    scanf("%s", tableName);
    char queryDrop[100];
    sprintf(queryDrop, "DROP TABLE IF EXISTS %s;", tableName);

    PGresult *res = PQexec(conn, queryDrop);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        exit_nicely(conn);
    }
    PQclear(res);
    char queryTable[100];
    sprintf(queryTable, "CREATE TABLE %s()", tableName);
    res = PQexec(conn, queryTable);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        exit_nicely(conn);
    }
    PQclear(res);
    char option;
    while(option == 's'){
        char columnName[100];
        char columnType[100];
        printf("Digite o nome da coluna: \n");
        scanf("%s", columnName);
        printf("Digite o tipo da coluna: \n");
        scanf("%s", columnType);
        char query[100];
        sprintf(query, "ALTER TABLE %s ADD COLUMN %s %s;", tableName, columnName, columnType);
        res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            exit_nicely(conn);
        }
        PQclear(res);
        printf("Deseja adicionar mais uma coluna? (s/n)\n");
        scanf("%c", &option);
        if(option == 'n'){
            break;
        }
    }
}

void inserirDados(PGconn *conn)
{
    char tableName[100];
    printf("Digite o nome da tabela: \n");
    scanf("%s", tableName);
    char query[100];
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
    printf("Number of columns: %d\n", num_columns);
    for (int i = 0; i < num_columns; i++)
    {
        printf("%s\n", PQfname(res, i));
    }
    PQclear(res);
    printf("Digite os dados a serem inseridos: \nExemplo: '1, 'João', 'Rua 1', '123456789'\n");
    char dados[200];
    scanf("%s", dados);
    char queryInsert[100];
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
    char tableName[100];
    printf("Digite o nome da tabela: ");
    scanf("%s", tableName);
    char query[100];
    sprintf(query, "SELECT nome_coluna, tipo_dado FROM information_schema.columns WHERE table_schema = 'public' AND table_name = '%s';", tableName);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Failed to get tables: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
}

void exibirLista(PGconn *conn)
{
    
}

void main(int argc, char const *argv[])
{
    PGconn *conn;
    PGresult *res;
    int option = 0;

    conn = PQconnectdb("host=localhost dbname=postgres user=postgres password=123456 port=5432");
    if (PQstatus(conn) != CONNECTION_OK)
    {
        printf("erro\n\n\n");
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit_nicely(conn);
    }
    //printf("Certo\n\n\n");

    while (!option)
    {
        printf("--Sistema de Banco de Dados Postgres--\n\n");
        printf("1 - Exibir lista de tabelas\n");
        printf("2 - Especificações de campos\n");
        printf("3 - Criar nova tabela\n");
        printf("4 - Inserir dados\n");
        printf("5 - Exibir dados\n");
        printf("6 - Remover dados\n");
        switch (option)
        {
        case 1:
            exibirLista(conn);
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
            /* code */
            break;
        case 6:
            /* code */
            break;
        default:
            break;
        }
    }
}
