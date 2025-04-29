IMAGE_TAG=${IMAGE_TAG:-1.0}
IMAGE_NAME=cpp-bfvars:${IMAGE_TAG}
CONTAINER_NAME=cpp-bfvars

docker build -t ${IMAGE_NAME} . 
docker run \
    --name ${CONTAINER_NAME} \
    --rm \
    -it \
    -v .:/app \
    ${IMAGE_NAME}
