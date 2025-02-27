<?php

$requestCount = 0;

// Создаем сокет для прослушивания входящих соединений
$socket = stream_socket_server("tcp://0.0.0.0:8080", $errno, $errstr);

if (!$socket) {
    die("Ошибка: $errstr ($errno)\n");
}

echo "Сервер запущен на порту 8080...\n";

while ($conn = stream_socket_accept($socket)) {
    // Увеличиваем счетчик запросов
    echo $requestCount;
    $requestCount++;

    // Формируем ответ
    $response = "HTTP/1.1 200 OK\r\n";
    $response .= "Content-Type: text/plain\r\n";
    $response .= "Connection: close\r\n\r\n";
    $response .= "Request number $requestCount has been processed";

    // Отправляем ответ клиенту
    fwrite($conn, $response);

    // Закрываем соединение
    fclose($conn);
}

fclose($socket);