import { promisify } from "util";
import { craRenderpipelineInit, RenderpipelineInitParams, WindowHandle } from "./libcrankshaft-node-binding";

const renderpipelineInit = promisify(craRenderpipelineInit)

export default class Renderpipeline {
  static async init(window: WindowHandle, params: RenderpipelineInitParams) {
    return await renderpipelineInit(window, params);
  }
}
