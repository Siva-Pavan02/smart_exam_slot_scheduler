const express = require('express');
const fs = require('fs');
const { execFile } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 3000;

app.use(cors());
app.use(express.json());
app.use(express.static(__dirname)); // to serve index.html
app.use(express.urlencoded({ extended: true }));

// Handle form input and run scheduler - FIXED ENDPOINT
app.post('/schedule', (req, res) => {
  const inputData = req.body.data; // FIXED: changed from req.body.input to req.body.data

  console.log('Received input:', inputData); // Debug log

  // Save input next to server file
  const inputPath = path.join(__dirname, 'input.txt');
  fs.writeFile(inputPath, inputData, (err) => {
    if (err) {
      console.error('Error writing input file:', err);
      return res.status(500).json({ error: 'Failed to save input file' });
    }

    console.log('Input file saved successfully'); // Debug log

    // Resolve executable path (Windows/Unix safe)
    const exePath = path.join(__dirname, 'exam_scheduler.exe');
    execFile(exePath, { windowsHide: true }, (error, stdout, stderr) => {
      if (error) {
        console.error('Execution error:', error);
        return res.status(500).json({ error: 'Failed to run scheduler: ' + error.message });
      }

      if (stderr) {
        console.error('STDERR:', stderr);
        // Don't return error for stderr unless stdout is empty
      }

      console.log('Scheduler output:', stdout); // Debug log

      // Success
      res.json({ output: stdout || 'Scheduler completed but produced no output' });
    });
  });
});

// Fallback endpoint for the original URL (backward compatibility)
app.post('/run-scheduler', (req, res) => {
  const inputData = req.body.input;
  
  const inputPath = path.join(__dirname, 'input.txt');
  fs.writeFile(inputPath, inputData, (err) => {
    if (err) {
      console.error('Error writing input file:', err);
      return res.status(500).json({ error: 'Failed to save input file' });
    }

    const exePath = path.join(__dirname, 'exam_scheduler.exe');
    execFile(exePath, { windowsHide: true }, (error, stdout, stderr) => {
      if (error) {
        console.error('Execution error:', error);
        return res.status(500).json({ error: 'Failed to run scheduler: ' + error.message });
      }

      if (stderr) {
        console.error('STDERR:', stderr);
      }

      res.json({ output: stdout || 'Scheduler completed but produced no output' });
    });
  });
});

// Health check endpoint
app.get('/health', (req, res) => {
  res.json({ status: 'Server is running' });
});

// Start server
app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});