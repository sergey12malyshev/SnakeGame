# Получаем текущий Git-тег
$tag = git describe --tags --abbrev=0

# Проверяем, установлен ли Git-тег
if ($tag -ne "") {
  # Создаем имя архива с использованием тега
  $archiveName = "$tag.zip"

  # Путь к файлу, который нужно архивировать
  $filePathHex = "build/pac_man_conf.hex"
  $filePathBin = "build/pac_man_conf.bin"

  # Архивируем файл в ZIP-архив
  Compress-Archive -Update -Path $filePathHex -DestinationPath $archiveName
  Compress-Archive -Update -Path $filePathBin -DestinationPath $archiveName

  # Вывод сообщения об успешном архивировании
  Write-Host "The file was successfully archived $archiveName"
} else {
  # Вывод сообщения об отсутствии тега
  Write-Host "Error: Git tag is not set. Unable to create archive!"
}