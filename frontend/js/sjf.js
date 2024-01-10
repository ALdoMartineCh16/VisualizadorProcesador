let sjfProcesses = [];
var interval;
var interval2;
var actual = 0;
var lastPosition;
let moX_SJF = 50;
var moY = 10;

class ProcessObject_SJF{
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
                    activateSJF.call(this);
                });
                
            }
        }
        function activateSJF(){
            this.referenceDocument.removeEventListener("transitionend", activateSJF); 
            midSJF();
        }
    }
}

function findMinor() {
    if (sjfProcesses.length == 0) {
        return -1;
    }
    let menor = sjfProcesses[0].burstTime;
    let index = 0;
    for (let i = 1; i < sjfProcesses.length; i++) {
        if (sjfProcesses[i].burstTime < menor) {
            menor = sjfProcesses[i].burstTime;
            index = i;
        }
    }
    return index;
}

function createBoxSJF(index,burst,name,pid){
    let divBox = document.createElement("div");
    divBox.classList.add("process", "bubbletext");
    sjfProcesses.push(new ProcessObject_SJF(divBox,burst,name,pid,900 + 120*index,175));
    return divBox;
}

function moveAllProcessesSFJ(){
    if(sjfProcesses.length - 1 == actual || sjfProcesses.length == 0 || (sjfProcesses.length>1 && sjfProcesses[actual+1].actualPosX <= lastPosition)){
        clearInterval(interval2);
        return;
    }
    for(let i = actual+1; i<sjfProcesses.length; i++){
        sjfProcesses[i].addToPos(-10,0);
    }   
}

function SJFAnimation(){
    if(sjfProcesses[actual].actualPosY >= 350){
        if(sjfProcesses[actual].actualPosX <= 450){
            clearInterval(interval);
            sjfProcesses[actual].consumeBurst();
        }else{
            sjfProcesses[actual].addToPos(-moX_SJF,0);
        }
        
    }else{
        sjfProcesses[actual].addToPos(0,moY); 
        if(sjfProcesses[actual].actualPosY >= 350){
            lastPosition = sjfProcesses[actual].actualPosX;
            interval2 = setInterval(moveAllProcessesSFJ,10);
        }
    }
}

function midSJF(){
    sjfProcesses[actual].referenceDocument.remove();
    sjfProcesses.splice(actual,1);
    if(0 == sjfProcesses.length){
        return;
    }
    interval = SJF();
}

function SJF(){
    if(0 == sjfProcesses.length){
        return null;
    }
    actual = findMinor();
    console.log(actual);
    let intervalo = setInterval(SJFAnimation,10);
    return intervalo;
}

// function fetchData(container) {
//     let index = 0;
//     return new Promise((resolve, reject) => {
//         fetch('http://localhost:8080/update_process_info')
//             .then(response => response.json())
//             .then(data => {
//                 data.forEach(process => {
//                     let nuevoDiv = SJF(index, process.burst_time.toFixed(4), process.name, process.pid);
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
//             interval = SJF();
//         })
//         .catch(error => console.error('Error:', error));
// });