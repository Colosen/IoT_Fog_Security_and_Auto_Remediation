const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');

const app = express();
const port = 3000; // You can change the port number if needed

// Middleware to parse JSON requests
app.use(bodyParser.json());

// Create a MySQL database connection
const db = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '123',
  database: 'iot',
});
db.connect((err) => {
  if (err) {
    console.error('Error connecting to MySQL:', err);
    return;
  }
 
});

// Route to handle POST requests
app.post('/api/submit', (req, res) => {
  // Check if the 'text' parameter exists in the request body
  if (!req.body) {
    return res.status(400).json({ error: 'Missing parameter: text' });
  }

  var inputText = req.body.packet;
  console.log(req.body);

  // var sql = `INSERT INTO packets_log VALUES ('${inputText[0]}','${inputText[1]}','${inputText[2]}','${inputText[3]}','${inputText[4]}','${inputText[5]}','${inputText[6]}','${inputText[7]}','${inputText[8]}','${inputText[9]}')`;

  // Connect to the database
  var sql = `INSERT INTO packets_log VALUES ('${inputText[0]}','${inputText[1]}','${inputText[2]}','${inputText[3]}','${inputText[4]}','${inputText[5]}','${inputText[6]}','${inputText[7]}','${inputText[8]}','${inputText[9]}')`;
  console.log('Connected to MySQL');
  db.query(sql, function (err, result) {
    if (err) throw err;
    console.log("1 record inserted");
  });

  // You can perform any desired operations with the inputText here
  // For now, we'll just echo it back in the response
  //res.json({ result: "{'gotit':'fine'}" });
  res.send({ title: 'GeeksforGeeks' });
});

// Define a route to retrieve and display records
app.get('/', (req, res) => {
  // Perform a SELECT query to retrieve records from the database
  db.query('SELECT * FROM packets_log', (err, results) => {
    if (err) {
      console.error('Database query error:', err);
      return res.status(500).send('Error fetching data from the database');
    }

    // Get the column names (record names) from the first record
    const columnNames = Object.keys(results[0]);

    // Render the records and data as HTML on the webpage
    const html = `
      <!DOCTYPE html>
      <html>
      <head>
        <title>Database Records</title>
      </head>
      <body>
        <h1>Database Records</h1>
        <table>
          <tr>
            ${columnNames.map((columnName) => `<th>${columnName}</th>`).join('')}
          </tr>
          ${results.map((record) => `
            <tr>
              ${columnNames.map((columnName) => `<td>${record[columnName]}</td>`).join('')}
            </tr>
          `).join('')}
        </table>
      </body>
      </html>
    `;

    res.send(html);
  });
});

// Define a route to retrieve and display records
app.get('/source_ip_count', (req, res) => {
  // Perform a SELECT query to retrieve records from the database
  db.query('SELECT DISTINCT SOURCE_IP, COUNT(*) FROM packets_log GROUP BY SOURCE_IP', (err, results) => {
    if (err) {
      console.error('Database query error:', err);
      return res.status(500).send('Error fetching data from the database');
    }

    // Get the column names (record names) from the first record
    const columnNames = Object.keys(results[0]);

    // Render the records and data as HTML on the webpage
    const html = `
      <!DOCTYPE html>
      <html>
      <head>
        <title>Database Records To Calculate PSR</title>
      </head>
      <body>
        <h1>Database Records</h1>
        <table>
          <tr>
            ${columnNames.map((columnName) => `<th>${columnName}</th>`).join('')}
          </tr>
          ${results.map((record) => `
            <tr>
              ${columnNames.map((columnName) => `<td>${record[columnName]}</td>`).join('')}
            </tr>
          `).join('')}
        </table>
      </body>
      </html>
    `;

    res.send(html);
  });
});

app.get('/destination_ip_count', (req, res) => {
  // Perform a SELECT query to retrieve records from the database
  db.query('SELECT DISTINCT DEST_IP, COUNT(*) FROM packets_log GROUP BY DEST_IP', (err, results) => {
    if (err) {
      console.error('Database query error:', err);
      return res.status(500).send('Error fetching data from the database');
    }

    // Get the column names (record names) from the first record
    const columnNames = Object.keys(results[0]);

    // Render the records and data as HTML on the webpage
    const html = `
      <!DOCTYPE html>
      <html>
      <head>
        <title>Database Records To Calculate PRR</title>
      </head>
      <body>
        <h1>Database Records</h1>
        <table>
          <tr>
            ${columnNames.map((columnName) => `<th>${columnName}</th>`).join('')}
          </tr>
          ${results.map((record) => `
            <tr>
              ${columnNames.map((columnName) => `<td>${record[columnName]}</td>`).join('')}
            </tr>
          `).join('')}
        </table>
      </body>
      </html>
    `;

    res.send(html);
  });
});

// Start the server
app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
