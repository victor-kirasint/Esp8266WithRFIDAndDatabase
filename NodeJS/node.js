const express = require("express");
const app = express();
const handlebars = require('express-handlebars')

//Define os models a ser usados
const consultar = require('./models/Consultacartao.js')
const gravarentrada = require('./models/Gravaentrada.js')
//fim
const bodyParser = require('body-parser')




app.use(bodyParser.urlencoded({extended: false}))
app.use(bodyParser.json())
//fim
//conectar mysql

//fim

app.get('/', async function(req, res){
    res.send("funcionando")
})

app.post('/teste', async function(req, res){
    var criar = await consultar.findOne({
        attributes:['nome'],
        where:{
            id:req.body.id
        }
    })
    var dados = await consultar.findAll({
        where:{
            id:req.body.id
        }
    })


    if (criar == null) {
        gravarentrada.create({
            nome:"Desconhecido"
        })
        res.send(dados);
    }
    else{
    gravarentrada.create({
        nome:criar.nome
    }).then(function(dados){
        res.send(dados)
    })
    }
    
})


app.listen(9010, function(){
    console.log("teste");
});