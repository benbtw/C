#include "chip8.h"

uint8_t chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
	
chip8 c;

void init_chip8(SDL_Renderer *renderer)
{
    c.delay_timer= 0;
	c.sound_timer= 0;
	c.opcode = 0;
	c.pc = 0x200;
	c.I = 0;
	c.sp = 0;
	memset(c.stack, 0, 16);
	memset(c.memory, 0, 4096);
	memset(c.V, 0, 16);
	memset(c.pixels, 0, 2048);
	memset(c.key, 0, 16);
	memcpy(c.memory, chip8_fontset, 80 * sizeof(int8_t));
    c.screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);
}

uint32_t loadRom(char* file)
{
    FILE *fp = fopen(file, "rb");
    if(fp == NULL)
    {
        fprintf(stderr, "Cannont open rom\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fread(c.memory + 0x200, sizeof(uint16_t), size, fp);
}

void chip8_run()
{
    c.opcode = c.memory[c.pc] << 8 | c.memory[c.pc + 1];   // Op code is two bytes

    switch(c.opcode & 0xF000){

        // 00E_
        case 0x0000:

            switch (c.opcode & 0x000F) {
                // 00E0 - Clear screen
                case 0x0000:
                    for (int i = 0; i < 2048; ++i) {
                        c.pixels[i] = 0;
                    }
                    c.drawFlag = true;
                    c.pc+=2;
                    break;

                // 00EE - Return from subroutine
                case 0x000E:
                    --c.sp;
                    c.pc = c.stack[c.sp];
                    c.pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %.4X\n", c.opcode);
                    exit(3);
            }
            break;

        // 1NNN - Jumps to address NNN
        case 0x1000:
            c.pc = c.opcode & 0x0FFF;
            break;

        // 2NNN - Calls subroutine at NNN
        case 0x2000:
            c.stack[c.sp] = c.pc;
            ++c.sp;
            c.pc = c.opcode & 0x0FFF;
            break;

        // 3XNN - Skips the next instruction if VX equals NN.
        case 0x3000:
            if (c.V[(c.opcode & 0x0F00) >> 8] == (c.opcode & 0x00FF))
                c.pc += 4;
            else
                c.pc += 2;
            break;

        // 4XNN - Skips the next instruction if VX does not equal NN.
        case 0x4000:
            if (c.V[(c.opcode & 0x0F00) >> 8] != (c.opcode & 0x00FF))
                c.pc += 4;
            else
                c.pc += 2;
            break;

        // 5XY0 - Skips the next instruction if VX equals VY.
        case 0x5000:
            if (c.V[(c.opcode & 0x0F00) >> 8] == c.V[(c.opcode & 0x00F0) >> 4])
                c.pc += 4;
            else
                c.pc += 2;
            break;

        // 6XNN - Sets VX to NN.
        case 0x6000:
            c.V[(c.opcode & 0x0F00) >> 8] = c.opcode & 0x00FF;
            c.pc += 2;
            break;

        // 7XNN - Adds NN to VX.
        case 0x7000:
            c.V[(c.opcode & 0x0F00) >> 8] += c.opcode & 0x00FF;
            c.pc += 2;
            break;

        // 8XY_
        case 0x8000:
            switch (c.opcode & 0x000F) {

                // 8XY0 - Sets VX to the value of VY.
                case 0x0000:
                    c.V[(c.opcode & 0x0F00) >> 8] = c.V[(c.opcode & 0x00F0) >> 4];
                    c.pc += 2;
                    break;

                // 8XY1 - Sets VX to (VX OR VY).
                case 0x0001:
                    c.V[(c.opcode & 0x0F00) >> 8] |= c.V[(c.opcode & 0x00F0) >> 4];
                    c.pc += 2;
                    break;

                // 8XY2 - Sets VX to (VX AND VY).
                case 0x0002:
                    c.V[(c.opcode & 0x0F00) >> 8] &= c.V[(c.opcode & 0x00F0) >> 4];
                    c.pc += 2;
                    break;

                // 8XY3 - Sets VX to (VX XOR VY).
                case 0x0003:
                    c.V[(c.opcode & 0x0F00) >> 8] ^= c.V[(c.opcode & 0x00F0) >> 4];
                    c.pc += 2;
                    break;

                // 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry,
                // and to 0 when there isn't.
                case 0x0004:
                    c.V[(c.opcode & 0x0F00) >> 8] += c.V[(c.opcode & 0x00F0) >> 4];
                    if(c.V[(c.opcode & 0x00F0) >> 4] > (0xFF - c.V[(c.opcode & 0x0F00) >> 8]))
                        c.V[0xF] = 1; //carry
                    else
                        c.V[0xF] = 0;
                    c.pc += 2;
                    break;

                // 8XY5 - VY is subtracted from VX. VF is set to 0 when
                // there's a borrow, and 1 when there isn't.
                case 0x0005:
                    if(c.V[(c.opcode & 0x00F0) >> 4] > c.V[(c.opcode & 0x0F00) >> 8])
                        c.V[0xF] = 0; // there is a borrow
                    else
                        c.V[0xF] = 1;
                    c.V[(c.opcode & 0x0F00) >> 8] -= c.V[(c.opcode & 0x00F0) >> 4];
                    c.pc += 2;
                    break;

                // 0x8XY6 - Shifts VX right by one. VF is set to the value of
                // the least significant bit of VX before the shift.
                case 0x0006:
                    c.V[0xF] = c.V[(c.opcode & 0x0F00) >> 8] & 0x1;
                    c.V[(c.opcode & 0x0F00) >> 8] >>= 1;
                    c.pc += 2;
                    break;

                // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's
                // a borrow, and 1 when there isn't.
                case 0x0007:
                    if(c.V[(c.opcode & 0x0F00) >> 8] > c.V[(c.opcode & 0x00F0) >> 4])	// VY-VX
                        c.V[0xF] = 0; // there is a borrow
                    else
                        c.V[0xF] = 1;
                    c.V[(c.opcode & 0x0F00) >> 8] = c.V[(c.opcode & 0x00F0) >> 4] - c.V[(c.opcode & 0x0F00) >> 8];
                    c.pc += 2;
                    break;

                // 0x8XYE: Shifts VX left by one. VF is set to the value of
                // the most significant bit of VX before the shift.
                case 0x000E:
                    c.V[0xF] = c.V[(c.opcode & 0x0F00) >> 8] >> 7;
                    c.V[(c.opcode & 0x0F00) >> 8] <<= 1;
                    c.pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %.4X\n", c.opcode);
                    exit(3);
            }
            break;

        // 9XY0 - Skips the next instruction if VX doesn't equal VY.
        case 0x9000:
            if (c.V[(c.opcode & 0x0F00) >> 8] != c.V[(c.opcode & 0x00F0) >> 4])
                c.pc += 4;
            else
                c.pc += 2;
            break;

        // ANNN - Sets I to the address NNN.
        case 0xA000:
            c.I = c.opcode & 0x0FFF;
            c.pc += 2;
            break;

        // BNNN - Jumps to the address NNN plus V0.
        case 0xB000:
            c.pc = (c.opcode & 0x0FFF) + c.V[0];
            break;

        // CXNN - Sets VX to a random number, masked by NN.
        case 0xC000:
            c.V[(c.opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (c.opcode & 0x00FF);
            c.pc += 2;
            break;

        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8
        // pixels and a height of N pixels.
        // Each row of 8 pixels is read as bit-coded starting from memory
        // location I;
        // I value doesn't change after the execution of this instruction.
        // VF is set to 1 if any screen pixels are flipped from set to unset
        // when the sprite is drawn, and to 0 if that doesn't happen.
        case 0xD000:
        {
            unsigned short x = c.V[(c.opcode & 0x0F00) >> 8];
            unsigned short y = c.V[(c.opcode & 0x00F0) >> 4];
            unsigned short height = c.opcode & 0x000F;
            unsigned short pixel;

            c.V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = c.memory[c.I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(c.pixels[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            c.V[0xF] = 1;
                        }
                        c.pixels[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            c.drawFlag = true;
            c.pc += 2;
        }
            break;

        // EX__
        case 0xE000:

            switch (c.opcode & 0x00FF) {
                // EX9E - Skips the next instruction if the key stored
                // in VX is pressed.
                case 0x009E:
                    if (c.key[c.V[(c.opcode & 0x0F00) >> 8]] != 0)
                        c.pc +=  4;
                    else
                        c.pc += 2;
                    break;

                // EXA1 - Skips the next instruction if the key stored
                // in VX isn't pressed.
                case 0x00A1:
                    if (c.key[c.V[(c.opcode & 0x0F00) >> 8]] == 0)
                        c.pc +=  4;
                    else
                        c.pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %.4X\n", c.opcode);
                    exit(3);
            }
            break;

        // FX__
        case 0xF000:
            switch(c.opcode & 0x00FF)
            {
                // FX07 - Sets VX to the value of the delay timer
                case 0x0007:
                    c.V[(c.opcode & 0x0F00) >> 8] = c.delay_timer;
                    c.pc += 2;
                    break;

                // FX0A - A key press is awaited, and then stored in VX
                case 0x000A:
                {
                    bool key_pressed = false;

                    for(int i = 0; i < 16; ++i)
                    {
                        if(c.key[i] != 0)
                        {
                            c.V[(c.opcode & 0x0F00) >> 8] = i;
                            key_pressed = true;
                        }
                    }

                    // If no key is pressed, return and try again.
                    if(!key_pressed)
                        return;

                    c.pc += 2;
                }
                    break;

                // FX15 - Sets the delay timer to VX
                case 0x0015:
                    c.delay_timer = c.V[(c.opcode & 0x0F00) >> 8];
                    c.pc += 2;
                    break;

                // FX18 - Sets the sound timer to VX
                case 0x0018:
                    c.sound_timer = c.V[(c.opcode & 0x0F00) >> 8];
                    c.pc += 2;
                    break;

                // FX1E - Adds VX to I
                case 0x001E:
                    // VF is set to 1 when range overflow (I+VX>0xFFF), and 0
                    // when there isn't.
                    if(c.I + c.V[(c.opcode & 0x0F00) >> 8] > 0xFFF)
                        c.V[0xF] = 1;
                    else
                        c.V[0xF] = 0;
                    c.I += c.V[(c.opcode & 0x0F00) >> 8];
                    c.pc += 2;
                    break;

                // FX29 - Sets I to the location of the sprite for the
                // character in VX. Characters 0-F (in hexadecimal) are
                // represented by a 4x5 font
                case 0x0029:
                    c.I = c.V[(c.opcode & 0x0F00) >> 8] * 0x5;
                    c.pc += 2;
                    break;

                // FX33 - Stores the Binary-coded decimal representation of VX
                // at the addresses I, I plus 1, and I plus 2
                case 0x0033:
                    c.memory[c.I]     = c.V[(c.opcode & 0x0F00) >> 8] / 100;
                    c.memory[c.I + 1] = (c.V[(c.opcode & 0x0F00) >> 8] / 10) % 10;
                    c.memory[c.I + 2] = c.V[(c.opcode & 0x0F00) >> 8] % 10;
                    c.pc += 2;
                    break;

                // FX55 - Stores V0 to VX in memory starting at address I
                case 0x0055:
                    for (int i = 0; i <= ((c.opcode & 0x0F00) >> 8); ++i)
                        c.memory[c.I + i] = c.V[i];

                    // On the original interpreter, when the
                    // operation is done, I = I + X + 1.
                    c.I += ((c.opcode & 0x0F00) >> 8) + 1;
                    c.pc += 2;
                    break;

                case 0x0065:
                    for (int i = 0; i <= ((c.opcode & 0x0F00) >> 8); ++i)
                        c.V[i] = c.memory[c.I + i];

                    // On the original interpreter,
                    // when the operation is done, I = I + X + 1.
                    c.I += ((c.opcode & 0x0F00) >> 8) + 1;
                    c.pc += 2;
                    break;

                default:
                    printf ("Unknown opcode [0xF000]: 0x%X\n", c.opcode);
            }
            break;

        default:
            printf("\nUnimplemented op code: %.4X\n", c.opcode);
            exit(3);
    }


    // Update timers
    if (c.delay_timer > 0)
        --c.delay_timer;

    if (c.sound_timer > 0)
        if(c.sound_timer == 1);
            // TODO: Implement sound
        --c.sound_timer;
}	

void chip8_keys(SDL_Event *event)
{
    switch(event->type)
	{
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym)
            {
                case SDLK_x: c.key[0] = 1;break;
                case SDLK_1: c.key[1] = 1;break;
                case SDLK_2: c.key[2] = 1;break;
                case SDLK_3: c.key[3] = 1;break;
                case SDLK_q: c.key[4] = 1;break;
                case SDLK_w: c.key[5] = 1;break;
                case SDLK_e: c.key[6] = 1;break;
                case SDLK_a: c.key[7] = 1;break;
                case SDLK_s: c.key[8] = 1;break;
                case SDLK_d: c.key[9] = 1;break;
                case SDLK_z: c.key[0xA] = 1;break;
                case SDLK_c: c.key[0xB] = 1;break;
                case SDLK_4: c.key[0xC] = 1;break;
                case SDLK_r: c.key[0xD] = 1;break;
                case SDLK_f: c.key[0xE] = 1;break;
                case SDLK_v: c.key[0xF] = 1;break;
            }
		break;
					
		case SDL_KEYUP:
		    switch (event->key.keysym.sym)
			{
				case SDLK_x: c.key[0] = 0;break;
				case SDLK_1: c.key[1] = 0;break;
				case SDLK_2: c.key[2] = 0;break;
				case SDLK_3: c.key[3] = 0;break;
				case SDLK_q: c.key[4] = 0;break;
				case SDLK_w: c.key[5] = 0;break;
				case SDLK_e: c.key[6] = 0;break;
				case SDLK_a: c.key[7] = 0;break;
				case SDLK_s: c.key[8] = 0;break;
				case SDLK_d: c.key[9] = 0;break;
				case SDLK_z: c.key[0xA] = 0;break;
				case SDLK_c: c.key[0xB] = 0;break;
				case SDLK_4: c.key[0xC] = 0;break;
				case SDLK_r: c.key[0xD] = 0;break;
				case SDLK_f: c.key[0xE] = 0;break;
				case SDLK_v: c.key[0xF] = 0;break;
			}
		break;
	}
}

void chip8_draw(SDL_Renderer *renderer)
{
    uint32_t display[64 * 32];
	unsigned int x, y;
		
	if (c.drawFlag)
	{
		memset(display, 0, (64 * 32) * 4);
		for(x = 0;x < 64;x++)
		{
			for(y = 0;y < 32;y++)
			{
				if (c.pixels[(x) + ((y) * 64)] == 1)
				{
					display[(x) + ((y) * 64)] = UINT32_MAX;
				}
			}
		}
		
		SDL_UpdateTexture(c.screen, NULL, display, 64 * sizeof(uint32_t));
	
		SDL_Rect position;
		position.x = 0;
		position.y = 0;
		position.w = 64 * 15;
		position.h = 32 * 15;
		SDL_RenderCopy(renderer, c.screen, NULL, &position);
		SDL_RenderPresent(renderer);
	}
	c.drawFlag = false;
}