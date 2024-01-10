let processes = [];
let interval;
let interval2;
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
        let tempBurst = this.burstTime;
        this.referenceDocument.addEventListener("transitionend", () => {
            rotateAndMinus.call(this);
        });
        this.rotate(360);
        function rotateAndMinus(){
            tempBurst--;
            this.referenceDocument.textContent = "BURST: " + tempBurst;
            if(tempBurst > 0){
                this.rotate(360 * (this.burstTime-tempBurst+1));
            }
            if(tempBurst == 0){
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

function createBox(id){
    let divBox = document.createElement("div");
    divBox.id = "process";
    divBox.className = "bubbletext";
    processes.push(new ProcessObject(divBox,id+3,900 + 120*id,175));
    return divBox;
}

function moveAllProcesses(){
    if(processes.length == 0 || (processes.length>1 && processes[1].actualPosX <= 900)){
        clearInterval(interval2);
        return;
    }
    for(let i = 1; i<processes.length; i++){
        processes[i].addToPos(-10,0);
    }   
}

function FIFOAnimation(){
    if(processes[0].actualPosY >= 295){
        if(processes[0].actualPosX <= 450){
            clearInterval(interval);
            processes[0].consumeBurst();
        }else{
            processes[0].addToPos(-moX,0);
        }
        
    }else{
        processes[0].addToPos(0,moY); 
        if(processes[0].actualPosY >= 295){
            interval2 = setInterval(moveAllProcesses,10);
        }
    }
}

function midFIFO(){
    processes[0].referenceDocument.remove();
    processes.shift();
    if(0 == processes.length){
        return;
    }
    interval = FIFO();
}

function FIFO(){
    if(0 == processes.length){
        return null;
    }
    let intervalo = setInterval(FIFOAnimation,10);
    return intervalo;
}

window.onload = function(){
    let container = document.getElementById("container");
    for(let i = 0; i<10; i++){
        let nuevoDiv = createBox(i);
        container.appendChild(nuevoDiv);
    }
    interval = FIFO();
}
