import { Server } from "@modelcontextprotocol/sdk/server/index.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { CallToolRequestSchema, ListToolsRequestSchema } from "@modelcontextprotocol/sdk/types.js";
import mqtt from "mqtt";

const client = mqtt.connect("mqtt://broker.hivemq.com:1883");
let ultimaDistancia = "Desconocida";

client.on("connect", () => {
    client.subscribe("casa/sensor/distancia");
});

client.on("message", (topic, message) => {
    if (topic === "casa/sensor/distancia") {
        ultimaDistancia = message.toString();
    }
});

const server = new Server(
    { name: "servidor-iot-claude", version: "1.0.0" },
    { capabilities: { tools: {} } }
);

server.setRequestHandler(ListToolsRequestSchema, async () => {
    return {
        tools: [
            {
                name: "leer_sensor_distancia",
                description: "Obtiene la última distancia medida por el sensor ultrasónico del objeto inteligente.",
                inputSchema: { type: "object", properties: {} }
            },
            {
                name: "controlar_leds",
                description: "Envía un comando para cambiar el estado de los LEDs del objeto inteligente.",
                inputSchema: {
                    type: "object",
                    properties: {
                        comando: {
                            type: "string",
                            enum: ["GREEN", "YELLOW", "RED", "DANGER"],
                            description: "El comando a enviar. GREEN (Seguro), YELLOW (Precaución), RED (Peligro), DANGER (Alerta máxima parpadeante)."
                        }
                    },
                    required: ["comando"]
                }
            }
        ]
    };
});

server.setRequestHandler(CallToolRequestSchema, async (request) => {
    if (request.params.name === "leer_sensor_distancia") {
        return {
            content: [{ type: "text", text: `La distancia actual del objeto es ${ultimaDistancia} cm.` }]
        };
    }

    if (request.params.name === "controlar_leds") {
        const comando = request.params.arguments.comando;
        client.publish("casa/led/control", comando);
        return {
            content: [{ type: "text", text: `Comando '${comando}' enviado exitosamente al objeto inteligente.` }]
        };
    }

    throw new Error("Herramienta no encontrada");
});

async function main() {
    const transport = new StdioServerTransport();
    await server.connect(transport);
    console.error("Servidor MCP de IoT iniciado correctamente");
}

main().catch(console.error);