#pragma once
#include <Arduino.h>
#include "config.h"

enum class DrawType : uint8_t { Start, Move, End, Clear, Unknown };

struct DrawMsg {
  DrawType type = DrawType::Unknown;
  int x = 0;
  int y = 0;
  String color = "#000000";   // #RRGGBB
  int thickness = 1;
  String tool = "pen";        // "pen" | "eraser"
};

// Vrací true když payload OK
inline bool parseDrawPayload(const String& payload, DrawMsg& out) {
  // payload: typ:x,y:#rrggbb:th:tool
  int c1 = payload.indexOf(':');
  int c2 = payload.indexOf(':', c1 + 1);
  int c3 = payload.indexOf(':', c2 + 1);
  int c4 = payload.indexOf(':', c3 + 1);
  int comma = payload.indexOf(',', c1 + 1);

  if (c1 < 1 || c2 <= c1 || c3 <= c2 || comma <= c1 || comma >= c2) return false;

  String typeStr = payload.substring(0, c1);
  if (typeStr == "start") out.type = DrawType::Start;
  else if (typeStr == "move") out.type = DrawType::Move;
  else if (typeStr == "end") out.type = DrawType::End;
  else if (typeStr == "clear") out.type = DrawType::Clear;
  else out.type = DrawType::Unknown;

  out.x = payload.substring(c1 + 1, comma).toInt();
  out.y = payload.substring(comma + 1, c2).toInt();
  out.color = payload.substring(c2 + 1, c3);

  if (c4 > c3) {
    out.thickness = payload.substring(c3 + 1, c4).toInt();
    out.tool = payload.substring(c4 + 1);
  } else {
    out.thickness = payload.substring(c3 + 1).toInt();
    out.tool = "pen";
  }

  if (out.thickness < 1) out.thickness = 1;
  if (out.thickness > 25) out.thickness = 25;

  // clamp do canvasu
  if (out.x < 0) out.x = 0; if (out.x > CANVAS_W - 1) out.x = CANVAS_W - 1;
  if (out.y < 0) out.y = 0; if (out.y > CANVAS_H - 1) out.y = CANVAS_H - 1;

  return true;
}
