import { promisify } from "util";
import { craRenderpipelineInit, CraRenderpipelineInitParams, WindowHandle } from "./libcrankshaft-node-binding";

const renderpipelineInit = promisify(craRenderpipelineInit)

export default class Renderpipeline {
  static async init(window: WindowHandle, params: CraRenderpipelineInitParams) {
    return await renderpipelineInit(window, params);
  }
}
