import json
import pymysql

def lambda_handler(event, context):
   resposta = processa_dados()
   if 'body' in resposta:
        return{
            'statusCode': resposta['statusCode'],
            'body': json.dumps(resposta['body'])
        }
   else:
       return{
           'statusCode': resposta['statusCode'],
       }


def processa_dados():
    teste = banco_dados()
    if teste != False:
        return{
                'statusCode': 200,
                'body':teste
            }
    else:
        return{
            'statusCode': 400,
        }
    pass


def banco_dados():
   # Configuração das informações de conexão do banco de dados
    db_host = "host-do-banco"
    db_user = "usuario-do-banco"
    db_password = "senha-do-banco"
    db_name = "nome-do-banco"

    try:
        conn = pymysql.connect(host=db_host, user=db_user, password=db_password, database=db_name)
        cursor = conn.cursor()

        # Executando a consulta no banco de dados
        query = "SELECT * FROM Botoes"
        cursor.execute(query)

        # Obtendo todos os dados da tabela
        dados_botoes = cursor.fetchall()

        # Fechando a conexão com o banco de dados
        cursor.close()
        conn.close()

        return dados_botoes

    except Exception as e:
        return False