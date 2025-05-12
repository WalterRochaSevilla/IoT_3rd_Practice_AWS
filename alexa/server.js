const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({endpoint: 'a2rmxu7hc5rdsd-ats.iot.us-east-2.amazonaws.com'});

// const TurnOffParams = {
//     thingName: 'IoT_thing',
//     payload: '{"state": {"desired": {"led_builtin": 0}}}',
// };

// const TurnOnParams = {
//     thingName: 'IoT_thing',
//     payload: '{"state": {"desired": {"led_builtin": 1}}}',
// };

const ShadowParams = {
    thingName: 'IoT_thing',
};

const OpenInteriorDoorParams = {
    thingName: 'IoT_thing',
    payload: '{"state": {"desired": {"interiorDoor": "OPEN"}}}',
};
const CloseInteriorDoorParams = {
    thingName: 'IoT_thing',
    payload: '{"state": {"desired": {"interiorDoor": "CLOSE"}}}',
};

function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject('Failed to get thing shadow ${err.errorMessage}');
            } else {
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        const speakOutput = 'Bienvenido a tu objeto inteligente, tienes las opciones de encender, apagar y consultar el estado, ¿Qué deseas hacer?';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};
const StateExteriorDoorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateExteriorDoorIntent';
    },
    async handle(handlerInput) {
        var speakOutput = "Error";
        const shadowPromise = getShadowPromise(ShadowParams);
        const shadow = await shadowPromise;
        const desired = shadow.state.desired;
        const exteriorDoor = desired.exteriorDoor;
        const stateDoor = exteriorDoor == "OPEN" ? "Abierta": "Cerrada" ;
        speakOutput = 'La puerta exterior esta ' + stateDoor;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }

}

const OpenInteriorDoorIntentHandler = {
    canHandle(handlerInput){
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'OpenInteriorDoorIntent';
    },
    handle(handlerInput){
        var speakOutput = "Error";
        IotData.updateThingShadow(OpenInteriorDoorParams, function(err, data) {
            if (err) console.log(err);
        });

        speakOutput = 'Solicitaste abrir la puerta Interior!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
}
const CloseInteriorDoorIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'CloseInteriorDoorIntent';
    },
    handle(handlerInput) {
        var speakOutput = "Error";
        IotData.updateThingShadow(CloseInteriorDoorParams, function(err, data) {
            if (err) console.log(err);
        });

        speakOutput = 'Solicitaste cerrar la puerta Interior!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
}

const StateInteriorDoorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateInteriorDoorIntent';
    },
    async handle(handlerInput) {
        var speakOutput = "Error";
        const shadowPromise = getShadowPromise(ShadowParams);
        const shadow = await shadowPromise;
        const desired = shadow.state.desired;
        const interiorDoor = desired.interiorDoor;
        const stateDoor = interiorDoor == "OPEN" ? "Abierta": "Cerrada" ;
        speakOutput = 'La puerta interior esta ' + stateDoor;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
}



// const TurnOnIntentHandler = {
//     canHandle(handlerInput) {
//         return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
//             && Alexa.getIntentName(handlerInput.requestEnvelope) === 'TurnOnIntent';
//     },
//     handle(handlerInput) {
//         var speakOutput = "Error";
//         IotData.updateThingShadow(TurnOnParams, function(err, data) {
//             if (err) console.log(err);
//         });
      
//         speakOutput = 'Solicitaste encender!';

//         return handlerInput.responseBuilder
//             .speak(speakOutput)
//             .reprompt(speakOutput)
//             .getResponse();
//     }
// };

// const TurnOffIntentHandler = {
//     canHandle(handlerInput) {
//         return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
//             && Alexa.getIntentName(handlerInput.requestEnvelope) === 'TurnOffIntent';
//     },
//     handle(handlerInput) {
//         var speakOutput = "Error";
//         IotData.updateThingShadow(TurnOffParams, function(err, data) {
//             if (err) console.log(err);
//         });
      
//         speakOutput = 'Solicitaste apagar!';

//         return handlerInput.responseBuilder
//             .speak(speakOutput)
//             .reprompt(speakOutput)
//             .getResponse();
//     }
// };

// const StateIntentHandler = {
//     canHandle(handlerInput) {
//         return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
//             && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateIntent';
//     },
//     async handle(handlerInput) {
//         var builtInLed = 'unknown';
//         await getShadowPromise(ShadowParams).then((result) => led_builtin = result.state.reported.led_builtin);
//         console.log(led_builtin);

//         var speakOutput = 'Error';
//         if (led_builtin == 1) {
//             speakOutput = 'El led está encendido';
//         } else if (led_builtin == 0) {
//             speakOutput = 'El led está apagado';
//         } else {
//             speakOutput = 'No se pudo consultar el estado del led, por favor intente más tarde';
//         }

//         return handlerInput.responseBuilder
//             .speak(speakOutput)
//             .reprompt(speakOutput)
//             .getResponse();
//     }
// };

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Tienes las opciones de encender, apagar y consultar el estado.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Hasta pronto!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};
/* *
 * FallbackIntent triggers when a customer says something that doesn’t map to any intents in your skill
 * It must also be defined in the language model (if the locale supports it)
 * This handler can be safely added but will be ingnored in locales that do not support it yet 
 * */
const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Lo siento, no entendí, intenta de nuevo.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};
/* *
 * SessionEndedRequest notifies that a session was ended. This handler will be triggered when a currently open 
 * session is closed for one of the following reasons: 1) The user says "exit" or "quit". 2) The user does not 
 * respond or says something that does not match an intent defined in your voice model. 3) An error occurs 
 * */
const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};
/* *
 * The intent reflector is used for interaction model testing and debugging.
 * It will simply repeat the intent the user said. You can create custom handlers for your intents 
 * by defining them above, then also adding them to the request handler chain below 
 * */
const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `Intentó ejecutar ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};
/**
 * Generic error handling to capture any syntax or routing errors. If you receive an error
 * stating the request handler chain is not found, you have not implemented a handler for
 * the intent being invoked or included it in the skill builder below 
 * */
const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Disculpa, hubo un error. Intenta de nuevo.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

/**
 * This handler acts as the entry point for your skill, routing all request and response
 * payloads to the handlers above. Make sure any new handlers or interceptors you've
 * defined are included below. The order matters - they're processed top to bottom 
 * */
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        StateExteriorDoorHandler,
        StateInteriorDoorHandler,
        OpenInteriorDoorIntentHandler,
        CloseInteriorDoorIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();