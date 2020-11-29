import { craLogSetLevel } from "./libcrankshaft-node-binding";

export default class Log {
  static setLevel(level: 0|1|2|3|4|5) {
    craLogSetLevel(level);
  }
}