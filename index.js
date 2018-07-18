const primitive = require ('./build/Release/vxanode');

module.exports.primitive = primitive;

module.exports.v1 = function (...args) {
    var prodCount=0;
    const prodId=setInterval(()=>{prodCount++},30);
    const stopProdding=r=>{
        clearInterval(prodId);
        return r;
    }
    return primitive.v1(...args).then (
        response=>stopProdding(response),
        response=>stopProdding(response)
    );
}

module.exports.v2 = function (...args) {
    return new Promise (
        (resolve,reject)=>primitive.v2(reject,resolve,...args)
    );
}

module.exports.o = function (...args) {
    return primitive.o2(
        s=>{
            console.log ("OnStop: ", ...args, s);
        }, ...args
    );
}

module.exports.v = module.exports.v2;
