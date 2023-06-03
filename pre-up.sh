export root=$(git rev-parse --show-toplevel)
echo $root
export mysqlIp=$(docker inspect -f '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mysql_container)
echo $mysqlIp
docker-compose build authentication && docker-compose up authentication
exec $SHELL -i #This will attach to the env
