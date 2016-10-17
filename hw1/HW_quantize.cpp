// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
// Written by: Alejandro Morejon Cortina, 2016
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;

    // compute lut[]
    int i,j, lut[MXGRAY];

    //with rounding the number of leves the user expects to see
    //is one more than what needs to be plugged in the formula
    //that's the reason for using levels -1
    double scale = MaxGray / (double) (levels -1);
    for (i=0; i< MXGRAY ; ++i)
    {
        //lut[i] = scale * (int) (i/ scale);

        //the new adjustment makes the result more balanced
        //altough the first and last steps will be half the size of the others
        lut[i] = ROUND(scale *  ROUND(i / scale));
    }

    int type, dithered, delta;
    ChannelPtr<uchar> p1, p2, endd;

    if(dither)
    {
        //add dither
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);

            //first do the even rows
            i=0;
            while(i < total)
            {
                //apply dither to row
                for(j=0;j<w;++j)
                {
                    delta = (qrand() & 0xf);//random int from 0 to 16
                    if(j & 1)
                    {
                        dithered = CLIP(*(p1 + i + j) + delta,0,MaxGray);
                    }else{
                        dithered = CLIP(*(p1 + i + j) - delta,0,MaxGray);
                    }

                    *(p2 + i +j) = lut[dithered];
                }

                //skip next row
                i = i + w + w;
            }

            //then do the odd rows
            i= w;
            while(i < total)
            {
                //apply dither to row
                for(j=0;j<w;++j)
                {
                    delta = (qrand() & 0xf);//random int from 0 to 16
                    if(j & 1)
                    {
                        dithered = CLIP(*(p1 + i + j) - delta,0,MaxGray);
                    }else{
                        dithered = CLIP(*(p1 + i + j) + delta,0,MaxGray);
                    }

                    *(p2 + i +j) = lut[dithered];
                }

                //skip next row
                i = i + w + w;
            }

        }


    }else{

        //no dither
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
        }

    }

}
