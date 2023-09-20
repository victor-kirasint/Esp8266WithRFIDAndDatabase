const db = require('./sequelize')

const consultacartao = db.sequelize.define('consultacartaos', {
    id:{
        type: db.Sequelize.INTEGER,
        autoIncrement: true,
        allowNull: false,
        primaryKey: true
    },
    nome:{
        type: db.Sequelize.CHAR
    }

    
    
})


module.exports = consultacartao