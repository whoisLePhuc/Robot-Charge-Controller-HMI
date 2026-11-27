# Pinned to the ESP-IDF release the project is built and configured with (sdkconfig.defaults,
# dependencies.lock). Change it together with those, never to "latest".
ARG DOCKER_TAG=v6.1
FROM espressif/idf:${DOCKER_TAG}

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update -y && apt-get install udev -y

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]

CMD ["/bin/bash", "-c"]