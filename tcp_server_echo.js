const net = require('net');

const port = 30000;
const host = "0.0.0.0";
const backlog = 511;

const server = net.createServer(function(conn) {
    console.log(`New client connected. IP: ${conn.remoteAddress}`); // we're inside server.on('connection')

    conn.on('data', (data) => {
        conn.write(data);
        console.log('echo');
    });

    conn.on('close', (had_error) => {
        if (had_error)
        {
            console.log('Connection closed with error. on_close error.');
        }
        else
        {
            console.log('Connection closed gracefully.');
        }
    });
});

server.listen(port, host, backlog, () => {
    console.log(`Server listening on port ${port}!`);
});
