if (!('serial' in window.navigator && TextDecoderStream)) {
  setErrorScreen();
}

const baudRate = 19200;

port = null;

writer = null;
reader = null;
inputTextDecoder = new TextDecoderStream();

readValueFull = '';

async function connect() {
  await selectPort();
}

async function selectPort() {
  port = await navigator.serial.requestPort();
  await port.open({ baudRate });

  setConnectedState();

  const encoder = new TextEncoderStream();
  outputDone = encoder.readable.pipeTo(port.writable);
  outputStream = encoder.writable;
  writer = outputStream.getWriter();

  port.readable.pipeTo(inputTextDecoder.writable);
  reader = inputTextDecoder.readable.getReader();

  setInterval(readLoop, 100);
}

function onAngleChange(motor, value) {
  startSingleControl(motor, value);
  setAngleLabel(motor, value);
}

function sendNumber(number) {
  writer.write(new Uint8Array([number]));
  writer.write('x');
  updateConsole('outputConsole', number);
}

function sendString(string) {
  writer.write(string);
  writer.write('x');
  updateConsole('outputConsole', string);
}

async function readLoop() {
  const { value, done } = await reader.read();
  if (done) {
    reader.releaseLock();
  }
  readValueFull += value;

  if (value[value.length - 1] === '\n') {
    updateConsole('inputConsole', readValueFull);
    readValueFull = '';
  }
}

function updateConsole(consoleId, value) {
  const console = document.getElementById(consoleId);

  if (console.innerText.length > 1000)
    console.innerText = console.innerText.slice(800);
  console.innerText += value;
  if (consoleId == 'outputConsole') console.innerHTML += '<br>';
}

function clearConsole(consoleId) {
  const console = document.getElementById(consoleId);
  console.innerHTML = '';
}

function sendManualInput() {
  const message = document.getElementById('inputField').value;
  sendString(message);
}

function getParalellControlParameters() {
  const parameters = [];
  const motors = document
    .getElementById('parallelControl')
    .querySelectorAll('.inputs');

  motors.forEach((motor) => {
    parameters.push({
      angle: motor.querySelectorAll('input')[0].value,
      travelTime: motor.querySelectorAll('input')[1].value,
    });
  });

  return parameters;
}

function startParallelControl() {
  const parameters = getParalellControlParameters();

  parameters.forEach((motor) => {
    sendString('parallel');
    sendNumber(motor.angle);
    sendNumber(motor.travelTime);
  });
}

function startSingleControl(motor, target) {
  sendString('single');
  sendNumber(motor);
  sendNumber(target);
}

function setErrorScreen() {
  document.getElementById('singleControl').style.display = 'none';
  document.getElementById('parallelControl').style.display = 'none';
  document.getElementById('error').style.display = 'block';
}

function setConnectedState() {
  document.getElementById('buttonContainer').style.display = 'none';

  const inputs = document.querySelectorAll('input');
  inputs.forEach((input) => (input.disabled = false));
}

function setAngleLabel(motor, value) {
  document.getElementsByClassName('angle-label')[motor - 1].innerHTML = value;
}

function setSingleControlScreen() {
  document.getElementById('parallelControl').setAttribute('hidden', true);
  document.getElementById('singleControl').removeAttribute('hidden');
  document.getElementById('singleSwitch').setAttribute('disabled', true);
  document.getElementById('parallelSwitch').removeAttribute('disabled');
}

function setParallelControlScreen() {
  document.getElementById('singleControl').setAttribute('hidden', true);
  document.getElementById('parallelControl').removeAttribute('hidden');
  document.getElementById('singleSwitch').removeAttribute('disabled');
  document.getElementById('parallelSwitch').setAttribute('disabled', true);
}
