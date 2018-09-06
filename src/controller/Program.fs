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
            let command = Byte.Parse(input)

            printf "Input steering value: "
            let value = Console.ReadLine()
            let angle = SByte.Parse(value)

            printfn "Send command: [%d %d]" command angle
            stream.WriteByte(command)
            stream.WriteByte(byte angle)
    0
