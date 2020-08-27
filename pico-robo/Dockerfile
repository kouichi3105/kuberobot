FROM python:latest

COPY pico-dance.py /app/

# 作業するディレクトリを変更
WORKDIR /app

RUN apt install curl

# pythonのパッケージをインストール
RUN pip install requests

#RUN useradd -m nobody

USER nobody

#CMD ["python" , "pico-dance.py" ]
