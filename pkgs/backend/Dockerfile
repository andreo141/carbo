FROM oven/bun

# Create app directory
WORKDIR /app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
COPY package*.json ./
RUN bun install

# Bundle app source
COPY . .

EXPOSE 3000
ENTRYPOINT [ "bun", "run", "main.ts" ]
