import { OpenAI } from "langchain/llms/openai";
import { HNSWLib } from "langchain/vectorstores/hnswlib";
import { OpenAIEmbeddings } from "langchain/embeddings/openai";
import { RecursiveCharacterTextSplitter } from "langchain/text_splitter";
import * as fs from "fs";
import {
  VectorStoreToolkit,
  createVectorStoreAgent,

} from "langchain/agents";
import 'dotenv/config';
// import './fetch-polyfill';
import fetch from 'node-fetch';
import "core-js";

const model = new OpenAI({ modelName: "gpt-3.5-turbo", openAIApiKey: process.env.OPENAI_API_KEY,temperature: 0 });
/* Load in the file we want to do question answering over */
const text = fs.readFileSync("data/200point7_11.txt", "utf8");
/* Split the text into chunks */
const textSplitter = new RecursiveCharacterTextSplitter({ chunkSize: 1000 });
const docs = await textSplitter.createDocuments([text]);
// , {
  // chunkHeader:"This file lists a number of power grid transmission lines and their information, including individual name, location cooridnates in WKT format (i.e., latitude and longitude of the source and target point), flow capacity, voltage level, actual flow. Each line represents one transimisson line. The trnasmission line name is defined by join its source and target node name with ' -- '. If the actual flow value is positive, the first part of its name is the name of the source node and the second part of its name is the name of the target node If the PF value is negative, the first part of its name is the name of the target node and the second part of its name is the name of the source node"
  // ,appendChunkOverlapHeader: true
// }
/* Create the vectorstore */
const vectorStore = await HNSWLib.fromDocuments(docs, new OpenAIEmbeddings());

/* Create the agent */
const vectorStoreInfo= {
  name: "power_grid_generation",
  description: "generation information of the power grid network",
  vectorStore,
};

const toolkit = new VectorStoreToolkit(vectorStoreInfo, model);
const agent = createVectorStoreAgent(model, toolkit);

const input = `List names and power generated by the top 5 generations that generate the most power`
// "Which is the most robust generation?"
// `List names of generations that do not have a power capacity higher than 10.  `
// `Which type of generation generate the most power?`
// `Which type of generation have the most generations?`
// `What is the max power generated by an individual generation in the dataset?`
// `List names of all the wind or coal generations`
// `How much power generated by the generation "Rantoul 2"`
// `How many generations in total? `
// `What is the closest generation to generation 'RANTOUL 2' in Eclidean distance? The heneration coordinate infomration is in wkt format (i.e., longitude and latitude). You need to calcualte the distance between generations with their coordinate information.`
// `What is the closest generation to generation 'RANTOUL 2' by Eclidean distance?`
// `What is the max flow capacity in the dataset?` 
// `Based on the flow capacity and actual flow pass through the transmission line, which transmission line you would`
// `List names and actual flow values of transmission lines that flow from the source node Ellsworth 2. (gives part of the answers but does not use the pf information)` 
// `What is the max flow capacity in the dataset?` (chunk size )
// `How many transmission lines with a flow capacity higher than 400?` (chunk size)
// `List names of all the generations with the power capacity higher than 100`
// `List names of all the wind generations`
// `How much power generated by the generation "Rantoul 2"`;
console.log(`Executing: ${input}`);

const result = await agent.call({ input });
console.log(`Got output ${result.output}`);
console.log(
  `Got intermediate steps ${JSON.stringify(result.intermediateSteps, null, 2)}`
);
