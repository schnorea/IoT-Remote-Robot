import json
import requests


def lambda_handler(event, context):
    path = event.get('path', '0xCA')

    cmd = path[1:]
    # create and format values for HTTPS request
    # ****** HERE Change the url to point to the IOTCore MQTT Broker ******
    endpoint = '***********-ats.iot.us-east-1.amazonaws.com'
    # May need to change this if you IoTCore MQTT Policy is different
    topic = 'robo/cmds'
    message = f'Robo cmd {cmd}'

    publish_url = 'https://' + endpoint + ':8443/topics/' + topic + '?qos=0'
    publish_msg = message.encode('utf-8')
    
    # ****** HERE Point to your cert and key files ******
    cert = '**********-certificate.pem.crt'
    key = '**********-private.pem.key'

    # make request
    try:
        publish = requests.request('POST',
                    publish_url,
                    data=publish_msg,
                    cert=[cert, key])
        print(event)
        print(context)
    except requests.RequestException as e:
        # Send some context about this error to Lambda Logs
        print(e)
        raise e

    return {
        'statusCode': 200,
        'headers': {
            'Access-Control-Allow-Headers': '*',
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Methods': 'OPTIONS,POST,GET'
        },
        'body': json.dumps('Hello from Lambda!')
    }
