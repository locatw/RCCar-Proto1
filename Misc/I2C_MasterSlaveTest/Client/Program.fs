open System
open System.Net.Sockets

[<EntryPoint>]
let main argv =
    printf "Input server ip address: "
    let serverIPAddress = Console.ReadLine()
    let client = new TcpClient(serverIPAddress, 54237)
    let stream = client.GetStream()

    let mutable quit = false
    while not quit do
        printf "Input command: "
        let input = Console.ReadLine()

        if input.ToLower() = "q" then
            quit <- true
        else
            let value = Byte.Parse(input)
            printfn "Send command: %d" value
            stream.WriteByte(value)
    0
