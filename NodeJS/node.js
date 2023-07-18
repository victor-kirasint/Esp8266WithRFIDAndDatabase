const express = require("express");
const app = express();
const handlebars = require('express-handlebars')
const consultar = require('./models/Consultacartao.js')
const bodyParser = require('body-parser')


//config handlebars template engine

app.use(bodyParser.urlencoded({extended: false}))
app.use(bodyParser.json())
//fim
//conectar mysql

//fim

app.get('/', function(req, res){
    res.send('funcionando')
})

app.post('/teste',function(req, res){
    var dados = consultar.findAll({
        where:{
            id: req.body.id
        }
    }).then(function(dados){
        res.send(dados)
    });
}
)


app.listen(9010, function(){
    console.log("teste");
});