const Sequelize = require("sequelize")
const sequelize = new Sequelize('rfid', 'root', '', {
    timezone:"-03:00",
    host:'localhost',
    dialect:'mysql'
})

module.exports = {
    Sequelize: Sequelize,
    sequelize: sequelize
}