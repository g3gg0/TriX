<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="/">
    <html>
      <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
      </head>
      <title>PPM Dump of: <xsl:value-of select="PPM/INFORMATION/VERSION" /></title>
      <div>
        <xsl:apply-templates select="PPM/INFORMATION/VERSION" />
        <xsl:apply-templates select="PPM/INFORMATION/LANG">
        </xsl:apply-templates>
        <xsl:apply-templates select="PPM/CHUNK" />
      </div>
    </html>
  </xsl:template>
  <xsl:template match="PPM/INFORMATION/VERSION">
    <SPAN STYLE="display: 'block'; font-family: 'arial'; color: '#008000'; font-weight: '600'; font-size: '22'; margin-top: '12pt'; text-align: 'center'">
      <xsl:value-of select="." />
    </SPAN>
    <BR />
  </xsl:template>
  <xsl:template match="PPM/INFORMATION/LANG">
    <SPAN STYLE="display: 'block'; font-family: 'arial'; color: '#000080'; font-weight: '400'; font-size: '20'; margin-top: '12pt'">Language Pack:  <xsl:value-of select="." /></SPAN>
    <BR />
  </xsl:template>
  <xsl:template match="PPM/CHUNK/VERSION">
    <SPAN STYLE="display: 'block'; font-family: 'arial'; color: '#008000'; font-weight: '600'; font-size: '20'; margin-top: '12pt'; text-align: 'center'">
      <xsl:value-of select="." />
    </SPAN>
  </xsl:template>
  <xsl:template match="PPM/CHUNK">
    <OL>
      <xsl:choose>
        <xsl:when test="INFORMATION/NAME/text() = 'TEXT'">
          <LI STYLE="display: 'list-item'; list-style-type: 'square'; font-family: 'arial'; color: '#ff0000'; font-weight: '300';  margin-left: '15pt'; margin-top: '12pt'; font-size: '26'">
            <xsl:value-of select="INFORMATION/NAME" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
            </xsl:value-of>
          </LI>
          <xsl:for-each select="SUBCHUNK">
            <xsl:if test="string-length(INFORMATION/NAME) &gt; 1">
              <LI STYLE="display: 'list-item'; list-style-type: 'square'; font-family: 'arial'; color: '#006f00'; font-weight: '300';  margin-left: '20pt'; margin-top: '12pt'; font-size: '16'">
                <xsl:value-of select="INFORMATION/NAME">
                </xsl:value-of>
              </LI>
              <OL STYLE="display: 'list-item'; list-style: 'decimal';  font-family: 'arial'; color: '#390000'; font-weight: '300';  margin-left: '20pt'; margin-top: '3pt'; font-size: '10'">
                <xsl:for-each select="STRINGS/STRING" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
                  <LI>String: "<xsl:value-of select="UTF8"></xsl:value-of>"</LI>
                </xsl:for-each>
              </OL>
            </xsl:if>
          </xsl:for-each>
        </xsl:when>
        <xsl:when test="string-length(INFORMATION/NAME) &gt; 1">
          <LI STYLE="display: 'list-item'; list-style-type: 'square'; font-family: 'arial'; color: '#ff0000'; font-weight: '300';  margin-left: '15pt'; margin-top: '12pt'; font-size: '26'">
            <xsl:value-of select="INFORMATION/NAME">
            </xsl:value-of>
          </LI>
          <xsl:apply-templates select="SUBCHUNK" />
        </xsl:when>
      </xsl:choose>
    </OL>
  </xsl:template>
  <xsl:template match="SUBCHUNK">
    <BR />
    <xsl:if test="string-length(INFORMATION/NAME) &gt; 1">
      <LI STYLE="display: 'list-item'; list-style-type: 'square'; font-family: 'arial'; color: '#006f00'; font-weight: '300';  margin-left: '20pt'; margin-top: '12pt'; font-size: '16'">
        <xsl:value-of select="INFORMATION/NAME" />
      </LI>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>