let processes = [];
let interval;
let interval2;
let actual = 0;
let lastPosition;
let quantum = 2;
let moX = 10;
let moY = 10;

class ProcessObject{
    constructor(referenceDocument,burstTime,actualPosX,actualPosY){
        this.burstTime = burstTime;
        this.actualPosX = actualPosX;
        this.actualPosY = actualPosY;
        this.referenceDocument = referenceDocument;
        this.updatePos();
        this.referenceDocument.textContent = "BURST: " + burstTime;
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
        this.referenceDocument.addEventListener("transitionend", () => {
            rotateAndMinus.call(this);
        });
        this.rotate(360);
        function rotateAndMinus(){
            if(tempQuantum > 0){
                tempQuantum--;
                tempBurst--;
                this.referenceDocument.textContent = "BURST: " + tempBurst;
                if(tempBurst > 0){
                    this.rotate(360 * (this.burstTime-tempBurst+1));
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

function createBox(id){
    let divBox = document.createElement("div");
    divBox.id = "process";
    divBox.className = "bubbletext";
    processes.push(new ProcessObject(divBox, parseInt(1 + Math.random() * (10 - 1)) ,900 + 120*id,175));
    return divBox;
}

function createBoxBackup(backup){
    let divBox = document.createElement("div");
    divBox.id = "process";
    divBox.className = "bubbletext";
    processes.push(new ProcessObject(divBox, backup.burstTime ,900 + 120*(processes.length-1),175));
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

function RobinAnimation(){
    if(processes[actual].actualPosY >= 295){
        if(processes[actual].actualPosX <= 450){
            clearInterval(interval);
            processes[actual].consumeBurst();
        }else{
            processes[actual].addToPos(-moX,0);
        }
        
    }else{
        processes[actual].addToPos(0,moY); 
        if(processes[actual].actualPosY >= 295){
            lastPosition = processes[actual].actualPosX;
            interval2 = setInterval(moveAllProcesses,10);
        }
    }
}

function midRobin(q){
    let nuevoDiv;
    if(q > 0){
        let backup = processes[actual];
        backup.burstTime = q;
        nuevoDiv = createBoxBackup(backup);
        container.appendChild(nuevoDiv);
    }
    processes[actual].referenceDocument.remove();
    processes.splice(actual,1);
    if(0 == processes.length){
        return;
    }
    interval = Robin();
}

function Robin(){
    if(0 == processes.length){
        return null;
    }
    let intervalo = setInterval(RobinAnimation,10);
    return intervalo;
}

window.onload = function(){
    let container = document.getElementById("container");
    for(let i = 0; i<10; i++){
        let nuevoDiv = createBox(i);
        container.appendChild(nuevoDiv);
    }
    interval = Robin();
}
