from pypdf import PdfReader
p=r"C:\Users\prohibit\Desktop\无线串口\ATK-MW1278D模块用户手册_V1.0.pdf"
r=PdfReader(p)
print('pages',len(r.pages))
for i in range(min(10,len(r.pages))):
    txt=(r.pages[i].extract_text() or '')
    print(f'--- page {i+1} ---')
    print(txt[:2000])
