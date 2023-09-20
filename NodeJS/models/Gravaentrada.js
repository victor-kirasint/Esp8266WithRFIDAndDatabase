const db = require('./sequelize');

const gravaEntrada = db.sequelize.define("gravaentradas",
    {
        nome: {
            type: db.Sequelize.CHAR,
            allowNull: false,
        }
    }
)

module.exports = gravaEntrada;

