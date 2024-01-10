let processes = [];
let interval;
let interval2;
let actual;
let lastPosition;
let moX = 50;
let moY = 10;

class ProcessObject{
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
    if (processes.length == 0) {
        return -1;
    }
    let menor = processes[0].burstTime;
    let index = 0;
    for (let i = 1; i < processes.length; i++) {
        if (processes[i].burstTime < menor) {
            menor = processes[i].burstTime;
            index = i;
        }
    }
    return index;
}

function createBox(index,burst,name,pid){
    let divBox = document.createElement("div");
    divBox.id = "process";
    divBox.className = "bubbletext";
    processes.push(new ProcessObject(divBox,burst,name,pid,900 + 120*index,175));
    return divBox;
}

function moveAllProcesses(){
    if(processes.length - 1 == actual || processes.length == 0 || (processes.length>1 && processes[actual+1].actualPosX <= lastPosition)){
        clearInterval(interval2);
        return;
    }
    for(let i = actual+1; i<processes.length; i++){
        processes[i].addToPos(-10,0);
    }   
}

function SJFAnimation(){
    if(processes[actual].actualPosY >= 350){
        if(processes[actual].actualPosX <= 450){
            clearInterval(interval);
            processes[actual].consumeBurst();
        }else{
            processes[actual].addToPos(-moX,0);
        }
        
    }else{
        processes[actual].addToPos(0,moY); 
        if(processes[actual].actualPosY >= 350){
            lastPosition = processes[actual].actualPosX;
            interval2 = setInterval(moveAllProcesses,10);
        }
    }
}

function midSJF(){
    processes[actual].referenceDocument.remove();
    processes.splice(actual,1);
    if(0 == processes.length){
        return;
    }
    interval = SJF();
}

function SJF(){
    if(0 == processes.length){
        return null;
    }
    actual = findMinor();
    console.log(actual);
    let intervalo = setInterval(SJFAnimation,10);
    return intervalo;
}
function fetchData(container) {
    let index = 0;
    return new Promise((resolve, reject) => {
        fetch('http://localhost:8080/update_process_info')
            .then(response => response.json())
            .then(data => {
                data.forEach(process => {
                    let nuevoDiv = createBox(index, process.burst_time.toFixed(4), process.name, process.pid);
                    container.appendChild(nuevoDiv);
                    index++;
                });
                resolve();
            })
            .catch(error => reject(error));
    });
}

document.addEventListener("DOMContentLoaded", function() {
    let container = document.getElementById("container");

    fetchData(container)
        .then(() => {
            console.log('fetchData completado, ejecutando el resto del código');
            interval = SJF();
        })
        .catch(error => console.error('Error:', error));
});