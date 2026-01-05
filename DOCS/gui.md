# GUI Library

Anis provides a built-in GUI engine that uses a JSX-like syntax for declaring layouts.

## Basic Structure

UI is rendered using the `render_gui` function (when imported from "gui").

```javascript
import { render_gui } from "gui";

const App = () => (
    <View width="100%" height="100%" padding="20">
        <Column gap="10">
            <Text value="Anis Dashboard" fontSize="24" color="#FFF" />
            <Button label="Click Me" onClick={() => println("Clicked!")} />
        </Column>
    </View>
);

render_gui(<App />);
```

## Primitive Components

### `<View>` / `<Column>` / `<Row>`
Layout containers.
- **Attributes**: `width`, `height`, `padding`, `margin`, `gap`, `flex`, `backgroundColor`, `alignItems`, `justifyContent`.

### `<Text>`
Renders text strings.
- **Attributes**: `value`, `fontSize`, `color`.

### `<Button>`
Clickable interactive element.
- **Attributes**: `label`, `onClick`.

### `<Textfield>`
Single-line text input.
- **Attributes**: `value`, `onInput`, `placeholder`.

### `<Image>`
Renders an image from file.
- **Attributes**: `src`, `width`, `height`.

### `<Scrollview>`
A container that allows scrolling of its children.
- **Attributes**: `width`, `height`.

## Styling
Currently, styling is done via inline attributes. Values can be pixels (`10px` or `10`) or percentages (`50%`). Colors are hex codes (`#RRGGBB`).
