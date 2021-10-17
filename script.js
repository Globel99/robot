const baudRate = 19200

port = null
writer = null

gamepad = null
buttonsCache = []
buttonsDiff = []

writer = null
reader = null
inputTextDecoder = new TextDecoderStream()

readValueFull = ""

if ("serial" in window.navigator) {

}else
{
  document.getElementById('error').style.display = 'block'
}



setInterval(() => {
  if(gamepad) {
    if(buttonsCache === [])
    {
      buttonsCache = window.navigator.getGamepads()[0].buttons
    }else
    {
      const buttons = window.navigator.getGamepads()[0].buttons

      buttons.forEach((button, index) => {
        if(button.value > 0) console.log(index)
      })

      buttonsCache = buttons 
    }
  }
}, 100)



async function connect(){
  await selectPort()
}

async function selectPort() {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: baudRate });

    setConnectedState()

    const encoder = new TextEncoderStream();
    outputDone = encoder.readable.pipeTo(port.writable);
    console.log(outputDone)
    outputStream = encoder.writable;
    writer = outputStream.getWriter();

    port.readable.pipeTo(inputTextDecoder.writable)
    reader = inputTextDecoder.readable.getReader()

    setInterval(readLoop, 100)
}

function onRangeChange(motor, event) {
  sendValue([String(motor), String(event.target.value), String(11)])
}

function setConnectedState() {
  document.getElementById('buttonContainer').style.display = 'none';

  const inputs = document.querySelectorAll('input')
  inputs.forEach(input => input.disabled = false)
}

async function sendValue(value, closeMessage = true) {
  console.log(value)
  writer.write(new Uint8Array(value))

  updateConsole("outputConsole", value)

  if (closeMessage) writer.write('x')
}

window.addEventListener("gamepadconnected", function(e) {
  console.log("Gamepad connected at index %d: %s. %d buttons, %d axes.",
    e.gamepad.index, e.gamepad.id,
    e.gamepad.buttons.length, e.gamepad.axes.length);

  console.log(e.gamepad)
  gamepad = e.gamepad

  document.getElementById('controller-overlay').style.display = 'block'
}); 

async function readLoop () {
  const { value, done } = await reader.read();
  if (done) {
      reader.releaseLock();
  }

  readValueFull += value 

  if(value[value.length - 1] === '\n')
  {
      console.log(readValueFull)
      updateConsole("inputConsole", readValueFull)
      readValueFull = ""
  }
}

function updateConsole(consoleId, value) {
  const console = document.getElementById(consoleId)
  
  console.innerText += value
  if(consoleId == "outputConsole") console.innerHTML += '<br>'
}

function clearConsole(consoleId) {
  const console = document.getElementById(consoleId)
  console.innerHTML = ''
}