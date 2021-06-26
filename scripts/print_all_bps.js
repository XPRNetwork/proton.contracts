const { JsonRpc } = require('@proton/js')

const rpc = new JsonRpc(['https://proton.eoscafeblock.com'])

const main = async () => {
    const { rows: producers } = await rpc.get_table_rows({
        code: 'eosio',
        scope: 'eosio',
        table: 'producers',
        limit: -1
    })
    const producersWithAuth = producers.map(producer => ({ actor: producer.owner, permission: 'active' }))
    console.log(JSON.stringify(producersWithAuth, null, 4))
}

main()