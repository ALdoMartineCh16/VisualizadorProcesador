let roundRobinProcesses = [];
var interval;
var interval2;
var actual = 0;
var lastPosition;
var quantum;
let moX_RR = 10;
var moY = 10;

class ProcessObject_RR{
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
        let tempQuantum = quantum;
        let tempBurst = this.burstTime;
        let timeBurst = 1;
        let limitPer = 1000;
        this.referenceDocument.addEventListener("transitionend", () => {
            rotateAndMinus.call(this);
        });
        this.rotate(360);
        function rotateAndMinus(){
            if(tempQuantum > 0){
                if(tempQuantum < limitPer){
                    limitPer = tempQuantum;
                }

                tempQuantum -= limitPer;
                tempBurst-= limitPer;

                tempBurst = tempBurst.toFixed(4);

                timeBurst++;
                this.burstText.textContent = "BURST: " + (tempBurst > 0 ? tempBurst: 0);
                if(tempBurst > 0){
                    this.rotate(360 * timeBurst);
                    if(tempQuantum <= 0){
                        deactivateCircles(this.referenceDocument);
                    }
                }else{
                    deactivateCircles(this.referenceDocument);
                }
            }
            else if(tempQuantum <= 0 || tempBurst >= 0){
                deactivateCircles(this.referenceDocument);
            }
            limitPer = 1000;
        }
        function activateRobin(referenceDocument){
            referenceDocument.removeEventListener("transitionend", activateRobin); 
            midRobin(tempBurst);
        }
        function deactivateCircles(referenceDocument){
            console.log("IM FINISH");
            referenceDocument.removeEventListener("transitionend", rotateAndMinus); 
            referenceDocument.style.opacity = 0;
            referenceDocument.addEventListener("transitionend", () => {
                activateRobin.call(this, referenceDocument);
            });
        }
    }
}

function createBoxRR(index,burst,name,pid){
    let divBox = document.createElement("div");
    divBox.classList.add("process", "bubbletext");
    roundRobinProcesses.push(new ProcessObject_RR(divBox,burst,name,pid,900 + 120*index,175));
    return divBox;
}

function createBoxBackup(backup){
    let divBox = document.createElement("div");
    divBox.classList.add("process", "bubbletext");
    roundRobinProcesses.push(new ProcessObject_RR(divBox, backup.burstTime, backup.name, backup.pid ,900 + 120*(roundRobinProcesses.length-1),175));
    return divBox;
}

function moveAllProcessesRR(){
    if(roundRobinProcesses.length - 1 == actual || roundRobinProcesses.length == 0 || (roundRobinProcesses.length>1 && roundRobinProcesses[actual+1].actualPosX <= lastPosition)){
        clearInterval(interval2);
        return;
    }
    for(let i = actual+1; i<roundRobinProcesses.length; i++){
        roundRobinProcesses[i].addToPos(-10,0);
    }   
}

function RobinAnimation(){
    if(roundRobinProcesses[actual].actualPosY >= 295){
        if(roundRobinProcesses[actual].actualPosX <= 450){
            clearInterval(interval);
            roundRobinProcesses[actual].consumeBurst();
        }else{
            roundRobinProcesses[actual].addToPos(-moX_RR,0);
        }
        
    }else{
        roundRobinProcesses[actual].addToPos(0,moY); 
        if(roundRobinProcesses[actual].actualPosY >= 295){
            lastPosition = roundRobinProcesses[actual].actualPosX;
            interval2 = setInterval(moveAllProcessesRR,10);
        }
    }
}

function midRobin(q){
    let nuevoDiv;
    if(q > 0){
        let backup = roundRobinProcesses[actual];
        backup.burstTime = q;
        nuevoDiv = createBoxBackup(backup);
        container.appendChild(nuevoDiv);
    }
    roundRobinProcesses[actual].referenceDocument.remove();
    roundRobinProcesses.splice(actual,1);
    if(0 == roundRobinProcesses.length){
        return;
    }
    interval = Robin();
}

function Robin(){
    if(0 == roundRobinProcesses.length){
        return null;
    }
    let intervalo = setInterval(RobinAnimation,10);
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
//             interval = Robin();
//         })
//         .catch(error => console.error('Error:', error));
// });