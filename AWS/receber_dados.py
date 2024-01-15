import json
import pymysql
import datetime
import pytz  

def lambda_handler(event, context):
    if 'queryStringParameters' in event:
        query_params = event['queryStringParameters']
        nome = query_params.get('botao')
        codigo = processa_dados(nome)
        return {
            'statusCode': codigo
        }
    return {
        'statusCode': 500,
    }

def processa_dados(botao):
    # Obter a hora atual com o fuso horário local
    hora_atual = datetime.datetime.now(pytz.timezone('America/Sao_Paulo')).time()
    hora_formatada = hora_atual.strftime("%H:%M")
    data_atual = datetime.datetime.now().date()
    data_formatada = data_atual.strftime("%Y-%m-%d")
    teste = banco_dados(botao, hora_formatada, data_formatada)
    if teste:
        return 200
    else:
        return 400

def banco_dados(nome, hora, data):
    db_host = "host-do-banco"
    db_user = "usuario-do-banco"
    db_password = "senha-do-banco"
    db_name = "nome-do-banco"
    try:
        conn = pymysql.connect(host=db_host, user=db_user, password=db_password, database=db_name)
        cursor = conn.cursor()
        query = "INSERT INTO Botoes (Botao, Hora, Data) VALUES (%s, %s, %s)"
        values = (nome, hora, data)
        cursor.execute(query, values)
        conn.commit()
        cursor.close()
        conn.close()
        return True
    except Exception as e:
        return False