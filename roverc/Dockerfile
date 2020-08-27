FROM python:latest

COPY roverc.py /app/

RUN apt install curl

RUN pip install requests

WORKDIR /app

USER nobody

#CMD ["python" , "roverc.py"]