port = null
writer = null

gamepad = null
buttonsCache = []
buttonsDiff = []

inputStream = null
outputStream = null

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
    await port.open({ baudRate: 9600 });

    setConnectedState()

    const encoder = new TextEncoderStream();
    outputDone = encoder.readable.pipeTo(port.writable);
    console.log(outputDone)
    outputStream = encoder.writable;
    writer = outputStream.getWriter();
}

function onRangeChange(motor, event) {
  sendValue(motor)
  sendValue(event.target.value)
}

function setConnectedState() {
  const connectButton = document.getElementById('connectButton');
  connectButton.innerText = 'KAPCSOLÓDVA'
  connectButton.disabled = true
  connectButton.style.color = 'lightgrey'

  document.getElementById('buttonContainer').style.height = '100px';

  const inputs = document.querySelectorAll('input')
  inputs.forEach(input => input.disabled = false)
}

async function sendValue(value, closeMessage = true){
  console.log(value)
  writer.write(new Uint8Array([value]))
  
  if(closeMessage) writer.write('x')
}

navigator.serial.addEventListener('connect', (e) => {
  // Connect to `e.target` or add it to a list of available ports.
});

window.addEventListener("gamepadconnected", function(e) {
  console.log("Gamepad connected at index %d: %s. %d buttons, %d axes.",
    e.gamepad.index, e.gamepad.id,
    e.gamepad.buttons.length, e.gamepad.axes.length);

  console.log(e.gamepad)
  gamepad = e.gamepad

  document.getElementById('controller-overlay').style.display = 'block'
}); 

