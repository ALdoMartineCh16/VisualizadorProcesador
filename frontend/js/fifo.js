let fifoProcesses = [];
var interval;
var interval2;
let moX_FIFO = 10;
var moY = 10;

class ProcessObject_FIFO{
    constructor(referenceDocument,burstTime,name,pid,actualPosX,actualPosY){
        this.name = name;
        this.pid = pid;
        this.burstTime = burstTime;
        this.actualPosX = actualPosX;
        this.actualPosY = actualPosY;
        this.referenceDocument = referenceDocument;
        this.updatePos();

        this.pidText = document.createElement('p');
        this.burstText = document.createElement('p');
        
        this.pidText.textContent = "PID: " + pid;
        this.burstText.textContent = "BURST: " + burstTime;

        this.referenceDocument.appendChild(this.pidText);
        this.referenceDocument.appendChild(this.burstText);
    }
    updatePos(){
        this.referenceDocument.style.left = this.actualPosX + 'px';
        this.referenceDocument.style.top = this.actualPosY + 'px';
    }
    addToPos(x,y){
        this.actualPosX += x;
        this.actualPosY += y;
        this.updatePos();
    }
    modifyPos(x,y){
        this.actualPosX = x;
        this.actualPosY = y;
        this.updatePos();
    }
    rotate(grados){
        this.referenceDocument.style.transform = "rotate(" + grados + "deg)";
    }
    consumeBurst(){
        let tempBurst = this.burstTime;
        let timeBurst = 1;
        this.referenceDocument.addEventListener("transitionend", () => {
            rotateAndMinus.call(this);
        });
        this.rotate(360);
        function rotateAndMinus(){
            tempBurst -= 1000;
            tempBurst = tempBurst.toFixed(4);
            timeBurst++;
            this.burstText.textContent = "BURST: " + (tempBurst > 0 ? tempBurst: 0);
            if(tempBurst > 0){
                this.rotate(360 * timeBurst);
            }
            if(tempBurst <= 0){
                console.log("IM FINISH");
                this.referenceDocument.removeEventListener("transitionend", rotateAndMinus); 
                this.referenceDocument.style.opacity = 0;
                this.referenceDocument.addEventListener("transitionend", () => {
                    activateFIFO.call(this);
                });
                
            }
        }
        function activateFIFO(){
            this.referenceDocument.removeEventListener("transitionend", activateFIFO); 
            midFIFO();
        }
    }
}

function createBoxFIFO(index,burst,name,pid){
    let divBox = document.createElement("div");
    divBox.classList.add("process", "bubbletext");
    fifoProcesses.push(new ProcessObject_FIFO(divBox,burst,name,pid,900 + 170*index,175));
    return divBox;
}

function moveAllProcessesFIFO(){
    if(fifoProcesses.length == 0 || (fifoProcesses.length>1 && fifoProcesses[1].actualPosX <= 900)){
        clearInterval(interval2);
        return;
    }
    for(let i = 1; i<fifoProcesses.length; i++){
        fifoProcesses[i].addToPos(-10,0);
    }   
}

function FIFOAnimation(){
    if(fifoProcesses[0].actualPosY >= 295){
        if(fifoProcesses[0].actualPosX <= 425){
            clearInterval(interval);
            fifoProcesses[0].consumeBurst();
        }else{
            fifoProcesses[0].addToPos(-moX_FIFO,0);
        }
        
    }else{
        fifoProcesses[0].addToPos(0,moY); 
        if(fifoProcesses[0].actualPosY >= 295){
            interval2 = setInterval(moveAllProcessesFIFO,10);
        }
    }
}

function midFIFO(){
    fifoProcesses[0].referenceDocument.remove();
    fifoProcesses.shift();
    if(0 == fifoProcesses.length){
        return;
    }
    interval = FIFO();
}

function FIFO(){
    if(0 == fifoProcesses.length){
        return null;
    }
    let intervalo = setInterval(FIFOAnimation,10);
    return intervalo;
}

// function fetchData(container) {
//     let index = 0;
//     return new Promise((resolve, reject) => {
//         fetch('http://localhost:8080/update_process_info')
//             .then(response => response.json())
//             .then(data => {
//                 data.forEach(process => {
//                     let nuevoDiv = createBox(index, process.burst_time.toFixed(4), process.name, process.pid);
//                     container.appendChild(nuevoDiv);
//                     index++;
//                 });
//                 resolve();
//             })
//             .catch(error => reject(error));
//     });
// }

// document.addEventListener("DOMContentLoaded", function() {
//     let container = document.getElementById("container");

//     fetchData(container)
//         .then(() => {
//             console.log('fetchData completado, ejecutando el resto del código');
//             interval = FIFO();
//         })
//         .catch(error => console.error('Error:', error));
// });