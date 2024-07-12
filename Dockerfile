FROM --platform=linux/amd64 alpine:3.20.1

WORKDIR /app

COPY . .

RUN ./build.sh

ENV PATH="/app/.venv/bin:$PATH"

ENV LD_LIBRARY_PATH=/app:$LD_LIBRARY_PATH

CMD ["sh", "-c", "source /app/.env && exec /app/test_libcsv"]
